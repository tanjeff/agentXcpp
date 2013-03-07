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
#include "util.hpp"

#include <qt4/QtCore/qthread.h>
#include <qt4/QtCore/QEventLoop>

using namespace agentxcpp;
using namespace std;


UnixDomainConnector::UnixDomainConnector(
        const std::string& _unix_domain_socket,
        unsigned _timeout)
: QObject(),
  socket(this),
  filename(QString::fromStdString(_unix_domain_socket)),
  timeout(_timeout)
{
    connect();

    //QObject::connect(&socket, SIGNAL(readyRead()), this, SLOT(receive()));
}


void UnixDomainConnector::connect()
{
    cout << "Connecting ... " << flush;
    switch(socket.state())
    {
        case QLocalSocket::UnconnectedState:
            socket.connectToServer(filename);
            break;
        case QLocalSocket::ConnectedState:
            // Already connected.
            cout << "OK" << endl;
            return;
        case QLocalSocket::ConnectingState:
            // Seems that last attempt for connection timed out
            // and is still unfinished. We will wait again.
            cout << "OK" << endl;
            break;
        case QLocalSocket::ClosingState:
            // Last disconnect seems still unfinished.
            cout << "FAIL" << endl;
            throw disconnected();
    }

    if(!socket.waitForConnected(timeout))
    {
        // timeout!
        cout << "timeout" << endl;
        throw disconnected();
    }
    cout << "OK" << endl;
    QObject::connect(&socket, SIGNAL(readyRead()), this, SLOT(receive()));
}


void UnixDomainConnector::disconnect()
{
    cout << "Disconnecting ... " << flush;
    socket.disconnectFromServer();
    if(!socket.waitForDisconnected(timeout))
    {
        cout << "timeout" << endl;
        // timeout
    }
    cout << "OK" << endl;
}


//bool UnixDomainConnector::is_connected()
//{
//    return socket.state() == QLocalSocket::ConnectedState;
//}


UnixDomainConnector::~UnixDomainConnector()
{
}


void UnixDomainConnector::receive()
{
    cout << "Data arrived, reading header ... " << flush;
    char header[20];
    if(socket.read(header, 20) != 20)
    {
        std::cout << "Could not read header" << std::endl;
        throw disconnected();
    }
    binary buf;
    buf.append(reinterpret_cast<uint8_t*>(header), 20);

    // read endianness flag
    bool big_endian = ( header[2] & (1<<4) ) ? true : false;

    // read payload length
    uint32_t payload_length;
    binary::const_iterator pos = buf.begin() + 16;
    payload_length = read32(pos, big_endian);
    if( payload_length % 4 != 0 )
    {
        // payload length must be a multiple of 4!
        // See RFC 2741, 6.1. "AgentX PDU Header"
        // -> close socket
        cout << "Invalid payload length." << endl;

        this->disconnect();
    }

    cout << buf.size() << " bytes, reading payload (expecting "
            << payload_length << " bytes) ... " << flush;

//    if(!socket.waitForReadyRead(this->timeout))
//    {
//        std::cout << "Timeout." << endl;
//        return;
//    }
    char* payload = new char[payload_length];
    qint64 bytes_read = socket.read(payload, payload_length);
    if(bytes_read != payload_length)
    {
        std::cout << "Could not read payload (" << bytes_read << ")." << std::endl;
        throw disconnected();
    }
    std::cout << "received " << bytes_read << " bytes ... " << flush;
    buf.append(reinterpret_cast<uint8_t*>(payload), payload_length);
    delete[] payload;

    cout << "packet has " << buf.size() << " in total: "
            << buf << " now parsing PDU ... " << flush;


    // Parse PDU
    shared_ptr<PDU> pdu;
    try
    {
        pdu = PDU::parse_pdu(buf);
    }
    catch(version_error)
    {
        cout << "version error." << endl;

    }
    catch(parse_error)
    {
        cout << "parse error." << endl;
        // disconnect
        this->disconnect();
    }



    // Special case: ResponsePDU's
    shared_ptr<ResponsePDU> response;
    response = boost::dynamic_pointer_cast<ResponsePDU>(pdu);
    if(response)
    {
        cout << "was a response ... " << flush;
        // Was a response
        std::map< uint32_t, boost::shared_ptr<ResponsePDU> >::iterator i;
        i = this->responses.find( response->get_packetID() );
        if(i != this->responses.end())
        {
            // Someone is waiting for this response
            cout << "inserting into response queue." << endl;
            i->second = response;
            response_arrived.wakeAll();
        }
        else
        {
            cout << "ignoring." << endl;
            // Nobody was waiting for the response
            // -> ignore it
        }
    }
    else
    {
        cout << "emitting pduArrived() ... " << flush;
        // Was not a Response
        // -> emit signal
        emit pduArrived(pdu);
        cout << "OK" << endl;
    }
}

boost::shared_ptr<ResponsePDU> UnixDomainConnector::request(boost::shared_ptr<PDU> pdu)
{
    cout << "Request in thread " << QThread::currentThread() << ": invoking send() ... " << flush;
    QMetaObject::invokeMethod(this, "send", Q_ARG(boost::shared_ptr<PDU>, pdu));

    cout << " ... " << flush;
    std::map<uint32_t, boost::shared_ptr<ResponsePDU> >::iterator i;
    cout << " ... " << flush;
    responses[pdu->get_packetID()] = shared_ptr<ResponsePDU>();
    do
    {
        cout << "waiting for response ... " << flush;
        response_arrived.wait(&response_arrival_mutex);
        cout << "something arrived, checking ... " << flush;
    }
    while ( ! (responses[pdu->get_packetID()]) );

    cout << "OK." << endl;
    shared_ptr<ResponsePDU> response = responses[pdu->get_packetID()];
    responses.erase(responses.find(pdu->get_packetID()));

    return response;
}



void UnixDomainConnector::send(boost::shared_ptr<PDU> pdu)
{
    binary data = pdu->serialize();

    socket.write(reinterpret_cast<const char*>(data.c_str()),
            data.size());
}
