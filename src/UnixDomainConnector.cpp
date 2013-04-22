/*
 * Copyright 2011-2012 Tanjeff-Nicolai Moos <tanjeff@cccmz.de>
 *
 * This file is part of the agentXcpp library.
 *
 * AgentXcpp is free software: you can redistribute it and/or modify
 * it under the terms of the AgentXcpp library license, version 1, which 
 * consists of the GNU General Public License and some additional 
 * permissions.
 *
 * AgentXcpp is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * See the AgentXcpp library license in the LICENSE file of this package 
 * for more details.
 */

#include "UnixDomainConnector.hpp"

#include <QtCore>
#include <QThread>
#include <QEventLoop>
#include <QMutexLocker>
#include <QScopedArrayPointer>

#include "util.hpp"

using namespace agentxcpp;
using namespace std;


UnixDomainConnector::UnixDomainConnector(
        const std::string& _unix_domain_socket,
        unsigned _timeout)
: QObject(),
  m_socket(this),
  m_filename(QString::fromStdString(_unix_domain_socket)),
  m_timeout(_timeout),
  m_is_connected(false)
{
    // We want to deliver this types within a signal:
    qRegisterMetaType< boost::shared_ptr<PDU> >("boost::shared_ptr<PDU>");
    qRegisterMetaType< boost::shared_ptr<PDU> >("shared_ptr<PDU>");

    QObject::connect(&m_socket, SIGNAL(readyRead()), this, SLOT(do_receive()));
}


void UnixDomainConnector::do_connect()
{
    // Inspect socket state
    switch(m_socket.state())
    {
        case QLocalSocket::UnconnectedState:
            // Currently not connected. Start connecting:
            m_socket.connectToServer(m_filename);
            break;
        case QLocalSocket::ConnectedState:
            // Already connected. Wake connect() and return.
            m_connection_waitcondition.wakeAll();
            return;
        case QLocalSocket::ConnectingState:
            // Seems that last attempt for connection timed out
            // and is still unfinished. We will wait again.
            break;
        case QLocalSocket::ClosingState:
            // Last disconnect seems still unfinished.
            // Wake connect() and return.
            m_connection_waitcondition.wakeAll();
            return;
    }

    // Protect m_is_connected
    QMutexLocker locker(&m_mutex_is_connected);

    // Wait for connection establishment
    if(m_socket.waitForConnected(m_timeout))
    {
        // OK: Set state to 'connected' and wake connect()
        m_is_connected = true;
        m_connection_waitcondition.wakeAll();
    }
    else
    {
        // Error: Set state to 'disconnected' and wake connect()
        m_is_connected = false;
        m_connection_waitcondition.wakeAll();
    }
}


bool UnixDomainConnector::connect()
{
    // Start do_connect()
    QMetaObject::invokeMethod(this, "do_connect");

    // Wait until do_connect() finishes
    m_connection_mutex.lock();
    m_connection_waitcondition.wait(&m_connection_mutex, m_timeout);
    m_connection_mutex.unlock();

    // Return status (i.e. whether connection was established)
    return is_connected();
}


void UnixDomainConnector::do_disconnect()
{
    // Disconnect
    m_socket.disconnectFromServer();
    if(!m_socket.waitForDisconnected(m_timeout))
    {
        // error: what to do?
        qDebug() << "Error while disconnecting from unix domain socket: "
                << m_socket.errorString();
    }

    // Update connection state
    QMutexLocker locker(&m_mutex_is_connected);
    m_is_connected = false; // Set this to false in any case

    // Wake disconnect()
    m_connection_waitcondition.wakeAll();
}


void UnixDomainConnector::disconnect()
{
    // Start do_disconnect()
    QMetaObject::invokeMethod(this, "do_disconnect");

    // Wait until do_disconnect() finishes
    m_connection_mutex.lock();
    m_connection_waitcondition.wait(&m_connection_mutex, m_timeout);
    m_connection_mutex.unlock();
}

bool UnixDomainConnector::is_connected()
{
    QMutexLocker locker(&m_mutex_is_connected);
    bool state = m_is_connected;

    return state;
}

UnixDomainConnector::~UnixDomainConnector()
{
}


void UnixDomainConnector::do_receive()
{
    // If a header was read from the socket, but the payload did not
    // yet arrive completely, the read header is stored here until
    // more data arrived:
    static binary last_header;

    // Read all PDUs into distinct buffers
    std::list<binary> queue;
    do
    {
        binary buf;

        // Read header
        if(last_header.size() != 0)
        {
            // Last time, we read a header, but no payload yet.
            // Therefore we don't read the header here, but continue
            // with reading the payload.
            buf = last_header;
            last_header.clear();
        }
        else
        {
            // No header read last time.
            char header[20];
            if(m_socket.read(header, 20) != 20)
            {
                disconnect(); // error!
                break; // Stop reading PDUs
            }
            buf.append(reinterpret_cast<uint8_t*>(header), 20);
        }

        // Extract endianness flag
        bool big_endian = ( buf[2] & (1<<4) ) ? true : false;

        // Extract payload length
        uint32_t payload_length;
        binary::const_iterator pos = buf.begin() + 16;
        payload_length = read32(pos, big_endian);
        if( payload_length % 4 != 0 )
        {
            // payload length must be a multiple of 4!
            // See RFC 2741, 6.1. "AgentX PDU Header"
            // We don't know where next PDU starts within the byte stream,
            // therefore we disconnect.
            disconnect(); // error!
            // stop reading PDU's (but process the ones we got so far)
            break;
        }

        // Read payload
        if(m_socket.bytesAvailable() < payload_length)
        {
            // Payload did not completely arrive. We store the header until
            // more data arrived:
            last_header = buf;
            break;
        }
        QScopedArrayPointer<char> payload(new char[payload_length]);
        qint64 bytes_read = m_socket.read(payload.data(), payload_length);
        if(bytes_read != payload_length)
        {
            disconnect();
            return;
        }
        buf.append(reinterpret_cast<uint8_t*>(payload.data()), payload_length);

        queue.push_back(buf);
    }
    while(m_socket.bytesAvailable() >= 20); // still enough data for next header

    // Process all received PDU's
    for(list<binary>::const_iterator i = queue.begin(); i != queue.end(); i++)
    {
        // Parse PDU
        shared_ptr<PDU> pdu;
        try
        {
            pdu = PDU::parse_pdu(*i);
        }
        catch(version_error)
        {
            return;
        }
        catch(parse_error)
        {
            return;
        }

        // Special case: ResponsePDU's
        shared_ptr<ResponsePDU> response;
        response = boost::dynamic_pointer_cast<ResponsePDU>(pdu);
        if(response)
        {
            // Was a response
            std::map< uint32_t, boost::shared_ptr<ResponsePDU> >::iterator i;
            i = this->m_responses.find( response->get_packetID() );
            if(i != this->m_responses.end())
            {
                // Someone is waiting for this response
                i->second = response;
                m_response_arrived.wakeAll();
            }
            else
            {
                // Nobody was waiting for the response
                // -> ignore it
            }
        }
        else
        {
            // Was not a Response
            // -> emit signal
            emit pduArrived(pdu);
        }
    }
}

boost::shared_ptr<ResponsePDU> UnixDomainConnector::request(boost::shared_ptr<PDU> pdu)
{
    QMetaObject::invokeMethod(this, "do_send", Q_ARG(boost::shared_ptr<PDU>, pdu));

    std::map<uint32_t, boost::shared_ptr<ResponsePDU> >::iterator i;
    m_responses[pdu->get_packetID()] = shared_ptr<ResponsePDU>();
    do
    {
        m_response_arrived.wait(&m_response_arrival_mutex);
    }
    while ( ! (m_responses[pdu->get_packetID()]) );

    shared_ptr<ResponsePDU> response = m_responses[pdu->get_packetID()];
    m_responses.erase(m_responses.find(pdu->get_packetID()));

    return response;
}



void UnixDomainConnector::do_send(boost::shared_ptr<PDU> pdu)
{
    binary data = pdu->serialize();

    m_socket.write(reinterpret_cast<const char*>(data.c_str()),
            data.size());
}


void UnixDomainConnector::send(boost::shared_ptr<PDU> pdu)
{
    // Start do_disconnect()
    QMetaObject::invokeMethod(this, "do_send", Q_ARG(boost::shared_ptr<PDU>, pdu));
}
