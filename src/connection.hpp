/*
 * Copyright 2011 Tanjeff-Nicolai Moos <tanjeff@cccmz.de>
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
#ifndef _LOCAL_SOCKET_H_
#define _LOCAL_SOCKET_H_

#include <boost/asio.hpp>

#include "ResponsePDU.hpp"

namespace agentxcpp
{
    class connection
    {
	private:

	    unsigned timeout;

	    /**
	     * \brief The mandatory io_service object.
	     *
	     * This object is needed for boost::asio sockets. Depending on the 
	     * constructor used, the object is either provided by the user or 
	     * generated automatically.
	     */
	    boost::asio::io_service* io_service;
	    
	    /**
	     * \brief The socket.
	     */
	    boost::asio::local::stream_protocol::socket socket;

	    /**
	     * \brief The endpoint used fo unix domain sockets.
	     */
	    boost::asio::local::stream_protocol::endpoint endpoint;
	    
	    /**
	     * \brief Callback function to read and process a %PDU
	     *
	     * This function is called when data is ready on the socket (the 
	     * header was already read into the header_buf buffer). It will 
	     * synchronously read one %PDU from the socket (to be precise, the 
	     * payload is read), process it and set up the next async read 
	     * operation, so that it is called again for new data.
	     *
	     * The synchronous read operation (to read the payload) may time 
	     * out. The timeout is the session's default timeout if available, 
	     * 1 second otherwise. If the read times out, the connection is 
	     * aborted (without notifying the master agent) and the 
	     * master_proxy object becomes disconnected.
	     * 
	     * Recieved ResponsePDU's are stored into the responses map if a 
	     * null pointer was stored there in advance.
	     */
	    void receive_callback(const boost::system::error_code& result);

	    /**
	     * \brief The received, yet unprocessed ReponsePDU's.
	     *
	     * The wait_for_response() function stores a null pointer to this 
	     * map to indicate that it is waiting for a certain response. The 
	     * map key is the packetID.
	     * 
	     * When a response is received, the receive_callback() function 
	     * stores it into the map, but only if a null pointer is found for 
	     * the packetID of the received ResponsePDU. Otherwise, the 
	     * received ResponsePDU is discarded.
	     *
	     * After a ResponsePDU was received and stored into the map, the 
	     * wait_for_response() function processes it and erases it from the 
	     * map.
	     */
	    std::map< uint32_t, boost::shared_ptr<ResponsePDU> > responses;

	    /**
	     * \brief Buffer to receive a PDU header
	     *
	     * When receiving a PDU asynchronously, the header is read into 
	     * this buffer. Then the receive_callback() callback is invoked, 
	     * which synchronously reads the payload and starts processing the 
	     * received %PDU.
	     *
	     * Since the AgentX-header is always 20 bytes in length, this 
	     * buffer is 20 bytes in size.
	     */
	    // TODO: avoid magic numbers, even if they are documented.
	    byte_t header_buf[20];

	    void (*handler)(shared_ptr<PDU>);
	    
	public:

	    connection(boost::asio::io_service* io_service,
				   const std::string& unix_domain_socket,
				   unsigned timeout);


	    /**
	     * \brief Wait with timeout for a reponse.
	     *
	     * This function blocks until a ResponsePDU with the given 
	     * packetID is received or until the timeout expires, whichever 
	     * comes first.  The received ResponsePDU (if any) is returned.
	     *
	     * This function calls run_one() on the io_service object until 
	     * the desired ResponsePDU arrives or the timeout expires. This 
	     * may cause other asynchronous operations to be served, as well.  
	     * As a side effect, the function may return later than the 
	     * timeout value requests.
	     *
	     * The received ResponsePDU's are put into the reponses map by the 
	     * receive_callback() function. This map is inspected by this 
	     * function, and the desired ResponsePDU is removed from the map 
	     * before returning it.
	     *
	     * \param packetID The packetID to wait for.
	     *
	     * \param timeout The timeout in seconds. The default is 0,
	     *                meaning "use the session's default timeout", or 1 
	     *                second if not default timeout is set.
	     *
	     * \exception timeout_exception If the timeout expired before the
	     *                              ResponsePDU was received. 
	     *
	     * \return The received ResponsePDU.
	     */
	    boost::shared_ptr<ResponsePDU>
		wait_for_response(uint32_t packetID,
				  unsigned timeout=0);

	    void register_handler( void (*handler)(shared_ptr<PDU>) );

	    void connect();

	    void disconnect();

	    bool is_connected()
	    {
		return this->socket.is_open();
	    }


    };
}

#endif

