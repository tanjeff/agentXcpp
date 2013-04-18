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
#ifndef _UNIX_DOMAIN_CONNECTOR_H_
#define _UNIX_DOMAIN_CONNECTOR_H_

#include <string>

#include <boost/shared_ptr.hpp>

#include <QObject>
#include <QLocalSocket>
#include <QWaitCondition>
#include <QMutex>
#include <QString>

#include "PDU.hpp"
#include "ResponsePDU.hpp"


namespace agentxcpp
{
    /**
     * \internal
     *
     * \brief Connect to a unix domain socket.
     *
     * This class connects to a unix domain socket and provides the following 
     * services:
     * - Methods to connect and disconnect to the unix domain socket, and
     *   a method to obtain the current state,
     * - A QT signal which is emitted when a PDU arrives (with an exception, 
     *   see below),
     * - A request service which sends a PDU and then blocks until the 
     *   corresponding ResponsePDU arrived,
     * - A send service which just sends a PDU.
     *
     * An object of this class is intended to run in its own thread, like so:
     * \code
     * QThread thread;
     * UnixDomainConnector connection = new UnixDomainConnector();
     * connection->moveToThread(&thread);
     * thread.start();
     * \endcode
     * The UnixDomainConnector object communicates with its "outside world" via 
     * the QT signal/slot mechanism (which is thread-safe). In addition, the 
     * class offers methods which can directly be called from any thread and 
     * which are also thread-safe (and often use the signal/slot mechanism 
     * internally).
     *
     * \par Connection handling
     *
     * The UnixDomainConnector class uses QLocalSocket to communicate over a 
     * unix domain socket. It always starts in disconnected state, which means 
     * that the QLocalSocket is disconnected. The private slots do_connect() 
     * and do_disconnect() are invoked to connect resp. disconnect the socket.  
     * However, these slots should not be invoked from outside the object 
     * (therefore they are private slots). The methods connect() and 
     * disconnect() are offered to handle connection. These methods invoke 
     * do_connect() resp. do_disconnect().
     *
     * The connect() method blocks until the connection is established or a 
     * timeout is detected. It does so by invoking do_connect(), then waiting 
     * for a QWaitCondition to be triggered. The disconnect() method works the 
     * same way.
     *
     * The connection state is tracked with the m_is_connected member, which is
     * protected by a mutex. The is_connected() method can access 
     * m_is_connected to inspect the state.
     */
    /**
     * \par Sending and Receiving PDU's
     *
     * AgentX is a protocol based on a request-response model. A subagent can 
     * send a request and wait for a response, while the master can also send 
     * requests and expect a response. Furthermore, a subagent or a master can 
     * send multiple requests, then wait for all the responses. All those 
     * %PDU's can be interleaved.
     *
     * The UnixDomainConnector class handles sending and receiving PDU's 
     * separately. Sending is done using the do_send() slot, which works for 
     * all types of PDU: all request-PDU's (such as OpenPDU) can be send 
     * without considering special cases, and ResponsePDU's also are no 
     * exception. Received PDU's are forwarded using the pduArrived() signal to 
     * whoever is listening, which works for PDU's \e except ResponsePDU's: 
     * these are the answer to a sent request-PDU and must be routed 
     * differently.
     *
     * Received ResponsePDU's are transmitted via the m_responses map. This map 
     * assigns a packetID a ResponsePDU. Each time a request is sent, do_send() 
     * adds an entry to the map with the packetID of the request and a NULL 
     * ResponsePDU (i.e. a NULL pointer). This entry indicates that a 
     * ResponsePDU with the same packetID is awaited. The do_receive() slot 
     * then adds the ResponsePDU to the map, when it arrived. However, when a 
     * ResponsePDU arrives which is \e not awaited, it is discarded.
     * 
     * \todo Improve error handling in all functions.
     */
    class UnixDomainConnector  : public QObject
    {
        Q_OBJECT

	private:

            /**
             * \brief The socket used internally for networking.
             */
	    QLocalSocket m_socket;

            /**
             * \brief The filename of the unix domain socket.
             */
	    QString m_filename;

	    /**
	     * \brief The timeout in milliseconds.
	     */
	    unsigned m_timeout;

            /**
             * \brief Needed for m_connection_waitcondition.
	     */
	    QMutex m_connection_mutex;

	    /**
             * \brief A waitcondition to synchronize connect actions.
	     *
             * This condition is used to synchronize connect() and do_connect()
             * respectively disconnect() and do_disconnect(). It is used in 
             * conjunction with m_connection_mutex.
	     */
	    QWaitCondition m_connection_waitcondition;

            /**
             * \brief Whether the object is currently connected.
             *
             * The member is protected by m_mutex_is_connected.
             */
            bool m_is_connected;

            /**
             * \brief A mutex to protect m_is_connected.
             */
            QMutex m_mutex_is_connected;

            /**
             * \brief Storage for ResponsePDU's.
             *
             * This map contains entries with packetID as key and 
             * %ResponsePDU's as values. An entry with a NULL pointer value 
             * means that a %ResponsePDU with the given packetID is awaited.
             *
             * This member is protected by m_response_mutex.
             */
	    std::map< quint32, boost::shared_ptr<ResponsePDU> > m_responses;

            /**
             * \brief Used to protect m_responses and for m_response_arrived.
             */
	    QMutex m_response_mutex;

            /**
             * \brief A waitcondition to inform waiters of ResponsePDU's.
             *
             * The m_response_mutex is used for synchronization.
             */
	    QWaitCondition m_response_arrived;

        private slots:

            /**
             * \brief Internal slot to receive data.
             *
             * This slot is connected to QLocalSocket::readyRead() and thus 
             * called when data arrives on the socket.
             *
             * The function reads as many complete %PDU's from the socket, 
             * parses them and invokes the pduArrived() signal for each %PDU, 
             * except for ResponsePDU's.
             *
             * %ResponsePDU's are stored to the m_responses map, if the map has 
             * an entry for the packetID of the received %ResponsePDU.  
             * Otherwise, the %ResponsePDU is discarded.
             *
             * Certain errors cause the UnixDomainConnector object to 
             * disconnect. Other errors are ignored and the respective PDU is 
             * discarded.
             *
             * \note Don't invoke this slot from outside the object!
             */
            void do_receive();

            /**
             * \brief Internal slot to send data.
             *
             * This slot is invoked when data must be sent. It serializes the 
             * given %PDU and sends it. Errors are ignored.
             *
             * \note Don't invoke this slot from outside the object!
             */
            void do_send(boost::shared_ptr<PDU> pdu);

            /**
             * \brief Connect to the remote entity.
             *
             * This function connects to the remote entity and waits
             * until connection is established, or until the timeout
             * expires.
             *
             * If the UnixDomainConnector is already connected, this function
             * does nothing. If the socket was disconnected, and the disconnect
             * operation is still in progress, this function also does nothing.
             *
             * If connecting times out, m_is_connected is set to false.
             *
             * If connecting succeeds, m_is_connected is set to true.
             *
             * After the work is done, m_connection_waitcondition.wakeAll()
             * is invoked in any case.
             *
             * \note Don't invoke this slot from outside the object!
             */
            void do_connect();

            /**
             * \brief Disconnect from a remote entity.
             *
             * This function disconnects from the remote entity and waits
             * until the operation finished, or until the timeout
             * expires.
             *
             * If disconnecting times out, m_socket may be left in state
             * ClosingState.
             *
             * m_is_connected is set to false in any case.
             *
             * conjunction with m_c
             * After the work is done, m_connection_waitcondition.wakeAll()
             * is invoked in any case.
             *
             * \note Don't invoke this slot from outside the object!
             */
            void do_disconnect();

	signals:
	    /**
	     * \brief Emitted when a PDU arrived.
             *
             * This signal is emitted once for every arrived PDU, except for 
             * %ResponsePDU's.
	     */
	    void pduArrived(shared_ptr<PDU>);

        public:
            /**
             * \brief Standard constructor.
             *
             * This constructor initializes the connector object to be in
             * disconnected state.
             *
             * \param unix_domain_socket The path to the unix_domain_socket.
             *
             * \param timeout The timeout, in milliseconds, used for for
             *                connecting, disconnecting, sending and receiving 
             *                %PDU's.  See the documentation of the respective 
             *                methods for details.
             */
            UnixDomainConnector(const std::string& unix_domain_socket
                                                   = "/var/agentx/master",
                                unsigned timeout = 1000);

            /**
             * \brief Destructor.
             */
            virtual ~UnixDomainConnector();

            /**
             * \brief Connect to the remote entity.
             *
             * This function connects to the remote entity and starts receiving
             * %PDU's.  If the object is already connected, the function does
             * nothing.
             *
             * For the attempt to connect, the configured timeout is used.
             *
             * This function invokes the do_connect() slot and then wait until 
             * m_connection_waitcondition is triggered (or a timeout is 
             * detected).
             *
             * \return True on success (i.e. if the object is in connected
             *         state), false otherwise.
             */
            bool connect();

            /**
             * \brief Disconnect from the remote entity.
             *
             * Stops receiving %PDU's and disconnects the remote entity.
             *
             * For the attempt to disconnect, the configured timeout is used.
             * 
             * This function invokes the do_disconnect() slot and then waits 
             * until m_connection_waitcondition is triggered (or a timeout is 
             * detected).
             *
             * The object will be in disconnected state after this method, no 
             * matter whether disconnecting times out or not.
             */
            void disconnect();

            /**
	     * \brief Find out whether the object is currently connected.
	     *
	     * \return True if the object is connected, false otherwise.
	     */
	    bool is_connected();

            /**
             * \brief Send a %PDU.
             *
             * This function enqueues a %PDU for sending, by invoking 
             * do_send(). This means the this function returns before the PDU 
             * is actually sent.
             *
             * \note Don't invoke do_send() yourself.
             */
	    void send(boost::shared_ptr<PDU> pdu);

            /**
             * \brief Send a PDU and wait for the response.
             *
             * This method sends a %PDU. Then, it adds an entry to m_responses 
             * to indicate that a ResponsePDU is awaited. Finally, it waits 
             * until that ResponsePDU arrives and returns it (it is removed 
             * from m_responses).
             *
             * \todo Add timeout. Currently, the method does not time out.
             */
	    boost::shared_ptr<ResponsePDU> request(boost::shared_ptr<PDU> pdu);

    };

}

#endif  //_UNIX_DOMAIN_CONNECTOR_H_

