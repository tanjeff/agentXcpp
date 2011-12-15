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

    /**
     * \internal
     *
     * \brief This class provides connection to another agentXcpp entity via
     *        unix domain socket.
     *
     * A connection object is always in one of the following states:
     *
     * -# connected
     * -# disconnected
     *
     * It tries to start in connected state. If that fails, it starts in 
     * disconnected state. The current state can also be obtained using the 
     * is_connected() method. Some operations may throw a disconnected 
     * exception if the object is in disconnected state. Further, the 
     * connection may fail at any point in time, therefore a disconnected 
     * exception may also be thrown during a send operation, for example.
     * 
     * Receiving %PDU's works as follows:
     *
     * - Opon connecting to the remote entity, an asynchronous read operation 
     *   is started to receive the %PDU header (fixed size).
     * - When an header arrived, the receive_callback() is invoked by 
     *   boost::asio, in the context of someone's io_service object's run() 
     *   method.
     * - The receive_callback() method reads the messages length from the 
     *   header and reveives the payload synchronously (but with a timeout).
     * - The receive_callback() method constructs a PDU object and calls a 
     *   user-provided handler callback (if one is registered), transferring 
     *   the newly created PDU object.
     * - The handler processes the %PDU as needed. As soon as the handler 
     *   finishes, the receive_callback() method starts the next asynchronous 
     *   read operation, so that it invoked again when the next header 
     *   arrived.
     * - If no handler was registered, the %PDU is received nevertheless and is 
     *   silently discarded.
     *
     * Sending works as follows:
     *
     * - The user invokes the send() method.
     * - The send() method sends the %PDU synchronously (but with a timeout).
     */
    class connection
    {
	private:

	    /**
	     * \brief the timeout in seconds.
	     *
	     * This value is used when sending a %PDU to the remote entity. 
	     * When sending doesn't complete within this timeout, it is 
	     * considered an error.
	     *
	     * While receiving a PDU, the value is also used. As soon as the 
	     * PDU header was received, the timeout is started. If the PDU is 
	     * not received completely after the timeout, it is considered an 
	     * error.
	     */
	    unsigned timeout;

	    /**
	     * \brief The mandatory io_service object.
	     *
	     * This object is needed for boost::asio sockets.
	     */
	    boost::shared_ptr<boost::asio::io_service> io_service;
	    
	    /**
	     * \brief The socket.
	     */
	    boost::asio::local::stream_protocol::socket socket;

	    /**
	     * \brief The endpoint used for unix domain sockets.
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

	    /**
	     * \brief Hide standard constructor
	     *
	     * We need an io_service object to function properly.
	     */
	    connection();
	    
	public:

	    /**
	     * \brief The constructor
	     *
	     * This constructor initializes the connection object and tries to 
	     * connect to the remote entity. If connection fails, the objects 
	     * starts in disconnected state.
	     *
	     * \param io_service The io_service object needed for boost::asio
	     *                   operations.
	     *
	     * \param unix_domain_socket The path to the unix_domain_socket.
	     *
	     * \param timeout The timeout, in seconds, for sending and
	     *                receiving %PDU's.  See the documentation of the 
	     *                respective methods for details.
	     */
	    connection(boost::shared_ptr<boost::asio::io_service> io_service,
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
	     *
	     * \internal
	     *
	     * The received ResponsePDU's are put into the reponses map by the 
	     * receive_callback() function. This map is inspected by this 
	     * function, and the desired ResponsePDU is removed from the map 
	     * before returning it.
	     */
	    boost::shared_ptr<ResponsePDU>
		wait_for_response(uint32_t packetID,
				  unsigned timeout=0);

	    /**
	     * \brief Register a callback handler for received %PDU's.
	     *
	     * Every time a %PDU is received, the callback will be invoked with 
	     * that %PDU as argument. The handler is executed in the context of 
	     * the io_service object's run() or run_one() method. Care should 
	     * been taken to not block the call, e.g. by doing some 
	     * networking.
	     *
	     * The handler can be the null pointer, in which case it is not 
	     * called. After registering a handler it can be unregistered again 
	     * by calling this function with a null pointer. While no handler 
	     * is registered, received %PDU's are silently discarded.
	     *
	     * There can be only one handler registered at a time.
	     *
	     * \param handler A pointer to the handler function, or null.
	     */
	    void register_handler( void (*handler)(shared_ptr<PDU>) );

	    /**
	     * \brief Connect to the remote entity.
	     *
	     * This function also starts receiving %PDU's. If the object is 
	     * already connected, the function does nothing.
	     * 
	     * \note While no handler is registered, received %PDU's are
	     *       silently discarded.
	     */
	    void connect();

	    /**
	     * \brief Disconnect the remote entity.
	     *
	     * This function also stops receiving %PDU's.
	     */
	    void disconnect();

	    /**
	     * \brief Find out whether the object is currently connected.
	     *
	     * \return Whether the object is in state connected.
	     */
	    bool is_connected()
	    {
		return this->socket.is_open();
	    }

	    /**
	     * \brief send a %PDU to the remote entity.
	     *
	     * The %PDU is sent to the remote entity. If the timeout expires 
	     * during the send operation, a timeout_error is thrown.
	     *
	     * \param pdu The PDU to send
	     *
	     * \exception timeout_error If a timeout error occurs. The function
	     *                          uses the timeout value given to the 
	     *                          constructor during construction.
	     */
	    void send(const PDU* pdu);


    };

}

#endif

