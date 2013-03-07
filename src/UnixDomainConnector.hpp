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

#include <boost/shared_ptr.hpp>
#include <string>
#include <qt4/QtNetwork/QLocalSocket>
#include <qt4/QtCore/QSharedPointer>
#include <qt4/QtCore/QWaitCondition>
#include <qt4/QtCore/QThread>
#include <qt4/QtCore/QMutex>

#include "PDU.hpp"
#include "ResponsePDU.hpp"


namespace agentxcpp
{
    class UnixDomainConnector  : public QObject
    {
        Q_OBJECT

	private:

	    QLocalSocket socket;

	    QString filename;

	    unsigned timeout;


	    std::map< uint32_t, boost::shared_ptr<ResponsePDU> > responses;

	    QMutex response_arrival_mutex;
	    QWaitCondition response_arrived;

	signals:
	    void pduArrived(shared_ptr<PDU>);

        public:

	    /**
	     * \brief The constructor
	     *
	     * This constructor initializes the connector object to be in 
	     * disconnected state.
	     *
	     * \param io_service The io_service object needed for boost::asio
	     *                   operations. It may also be used by other parts 
	     *                   of the program.
	     *
	     * \param unix_domain_socket The path to the unix_domain_socket.
	     *
	     * \param timeout The timeout, in milliseconds, for sending and
	     *                receiving %PDU's.  See the documentation of the 
	     *                respective methods for details.
	     *
	     * \exception None.
	     */
	    UnixDomainConnector(const std::string& unix_domain_socket = "/var/agentx/master",
	                          unsigned timeout = 1000);


            boost::shared_ptr<ResponsePDU> request(boost::shared_ptr<PDU> pdu);


        public slots:
	    /**
	     * \brief Connect to the remote entity.
	     *
	     * This function connects to the remote entity and starts receiving 
	     * %PDU's.  If the object is already connected, the function does 
	     * nothing.
	     * 
	     * \note While no handler is registered, received %PDU's are
	     *       silently discarded.
	     *
	     * \exception disconnected If connecting fails.
	     */
	    void connect();

	    /**
	     * \brief Disconnect the remote entity.
	     *
	     * Stops receiving %PDU's and disconnects the remote entity.
	     *
	     * \exception None.
	     */
	    void disconnect();




        private slots:

            void receive();
            void send(boost::shared_ptr<PDU> pdu);

        public:

	    /**
	     * \brief Find out whether the object is currently connected.
	     *
	     * \return Whether the object is in state connected.
	     *
	     * \exception None.
	     */
	    //bool is_connected();

	    /**
	     * \brief Destructor
	     *
	     * \exception None.
	     */
	    ~UnixDomainConnector();




	    boost::shared_ptr<PDU> wait_for_request();


    };

}

#endif  //_UNIX_DOMAIN_CONNECTOR_H_

