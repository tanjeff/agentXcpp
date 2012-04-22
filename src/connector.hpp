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
     *        a unix domain socket.
     *
     * A connector object is always in one of the following states:
     *
     * -# connected
     * -# disconnected
     *
     * When created, a connector object starts in disconnected state. The 
     * current state can be obtained using the is_connected() method. Some 
     * operations may throw a disconnected exception if the object is in 
     * disconnected state.  Further, the connection may fail at any point in 
     * time, therefore a disconnected exception may also be thrown during a 
     * network operation.
     * 
     * The socket needed for networking is created upon connect and destroyed 
     * upon disconnect. The reason is that closing the socket may throw a 
     * system_error exception, leaving the socket in an unknown state.  
     * Destroying it and creating a new one should be safe.
     * 
     * Sending works as follows:
     *
     * - The user invokes the send() method.
     * - The send() method sends the %PDU synchronously (but with a timeout).
     *
     * Note: The send() function invokes io_service->run_one() one or several
     *       times.
     *
     * Receiving %PDU's works as follows:
     *
     * - Upon connecting to the remote entity, an asynchronous read operation 
     *   is started to receive the %PDU header (fixed size). On disconnect, the 
     *   asynchronous read operation is stopped again.
     * - The boost::asio library writes a fixed amount of data (which is the 
     *   %PDU header) into the header_buf member and invokes the
     *   receive_callback() function.
     * - The receive_callback() method reads the payload length from header_buf 
     *   and reveives the payload synchronously (but with a timeout).
     * - The receive_callback() method then constructs a PDU object and 
     *   delivers it to a user-provided handler object (which implements the 
     *   connector::pdu_handler interface) by calling its handle_pdu() 
     *   method.
     * - The handler object processes the %PDU as needed. As soon as processing 
     *   finishes, the receive_callback() method starts the next asynchronous 
     *   read operation, so that it invoked again when the next header arrives.
     *
     * \note As a special case, ResponsePDU's are not delivered to the
     *       registered handler object, but are handled differently. see below 
     *       for details.
     *
     * \note If no handler object is registered, the %PDU is received
     *       nevertheless and is silently discarded.
     *
     * \note The receive_callback() function, and thus the registered handler
     *       object's handle_pdu() method are executed in the io_service's 
     *       run() context.
     */
    /**
     * Receiving %ResponsePDU's works as follows:
     *
     * The function wait_for_response() supports the request-response 
     * communication model. After sending a request to the remote entity (using 
     * send()), wait_for_response() is used to wait for the response. It blocks 
     * while waiting for a responsePDU from the remote entity, which is then 
     * returned to the caller. The wait_for_response() function may seem to do 
     * synchronous operations only, but this is not true.  In fact, it uses an 
     * asynchronous receive mechanism, because there may be other %PDU's in the 
     * queue before the given response is actually received.  
     * wait_for_response() therefore invokes io_service->run_one() one ore more 
     * times, until the response is received. This may also cause other 
     * asynchronous operations to finish.  For example, the registered handler 
     * object may receive other %PDU types, or another asynchronous operation 
     * on the io_service object (outside this class or even outside the 
     * agentXcpp library) may be served. Here are the steps performed to 
     * receive a ResponsePDU:
     *
     * - The wait_for_response() function puts an empty boost::shared_ptr<> 
     *   into the responses map, using the PacketID of the awaited ResponsePDU 
     *   as key.
     * - The wait_for_response() function invokes io_service->run_one() one or 
     *   several times, which triggers receive_callback() if data becomes 
     *   available.
     * - When the receive_callback() function is invoked, it receives a single 
     *   %PDU and processes it as described above.
     * - If a ResponsePDU is received, the registered handler object is not 
     *   informed by receive_callback(). Instead, the responses map is searched 
     *   for an entry with the same PacketID as the received ResponsePDU. If 
     *   found, the received ResponsePDU is stored in the map.  Otherwise the 
     *   ResponsePDU is silently discarded (as nobody waits for it). 
     * - The wait_for_response() checks its map entry after each run_one() call 
     *   for a received ResponsePDU. If it finds one, the entry is erased from 
     *   the map and returned to the caller.
     *
     * The same timeout value is used by all operations which deal with 
     * timeouts. The value is stored in the timeout member.
     */
    class connector
    {
	public:

	    /**
	     * \brief Interface for classes which can handle incoming PDU's.
	     *
	     * Classes which want to receive incoming %PDU's implement this 
	     * interface. An object of type pdu_handler can then be registered 
	     * with a connector object to indicate that it wants to receive 
	     * %PDU's.
	     */
	    class pdu_handler
	    {
		public:
		    /**
		     * \brief Handler method for incoming %PDU's.
		     *
		     * When a %PDU is received by the connector class, this 
		     * method is called on the registered object. Note that 
		     * ResponsePDU's are not handed over to the registered 
		     * object.
		     *
		     * The 'error' parameter is used to report errors. Using 
		     * exceptions is not possible because of the asynchronous 
		     * nature of the callback-mechanism.
		     *
		     * \param pdu The PDU which was just received. This is an
		     *            empty shared_ptr if an error occured.
		     *
		     * \param error The error code. 0 means success, -1 means
		     *              "parse error", -2 means "version error".
		     *
		     * \todo Check whether usage of boost::system makes
		     *       sense for error reporting.
		     */
		    virtual void handle_pdu(boost::shared_ptr<PDU> pdu,
					    int error) =0;
	    };
	
	private:

	    /**
	     * \brief The timeout in milliseconds, used in various contexts.
	     */
	    unsigned timeout;

	    /**
	     * \brief The mandatory io_service object.
	     *
	     * This object is needed for boost::asio sockets. It is provided by 
	     * the user of this class.
	     */
	    boost::shared_ptr<boost::asio::io_service> io_service;
	    
	    /**
	     * \brief The socket.
	     *
	     * The null pointer while disconnected.
	     */
	    boost::asio::local::stream_protocol::socket* socket;

	    /**
	     * \brief The endpoint used for unix domain sockets.
	     */
	    boost::asio::local::stream_protocol::endpoint endpoint;
	    
	    /**
	     * \brief Callback function to receive a %PDU.
	     *
	     * See the class documentation to learn about the receive 
	     * mechanism.
	     *
	     * \note Exceptions thrown by the user-provided handler (if any)
	     *       are catched and discarded.
	     * 
	     * The synchronous read operation (to read the payload) may time 
	     * out, using the class' timeout value. If the read times out, the 
	     * socket is destroyed and the connector object becomes 
	     * disconnected.
	     *
	     * \param result The result of the asynchronous read operation
	     *               (provided by boost::asio).
	     *
	     * \exception None.
	     */
	    void receive_callback(const boost::system::error_code& result);

	    /**
	     * \brief The received, yet unprocessed ReponsePDU's.
	     *
	     * See the class documentation to learn about the receive 
	     * mechanism.
	     *
	     * The wait_for_response() function stores a null pointer to this 
	     * map to indicate that it is waiting for a certain response.
	     *
	     * When a response is received, the receive_callback() function 
	     * stores it into the map, but only if a null pointer is found for 
	     * the packetID of the received ResponsePDU. Otherwise, the 
	     * received ResponsePDU is discarded.
	     *
	     * After a ResponsePDU was received and stored into the map, the 
	     * wait_for_response() function processes it and erases it from the 
	     * map.
	     *
	     * The map key is the packetID of the response which is awaited.
	     */
	    std::map< uint32_t, boost::shared_ptr<ResponsePDU> > responses;

	    /**
	     * \brief Buffer to receive a PDU header.
	     *
	     * See the class documentation to learn about the receive 
	     * mechanism.
	     *
	     * The %PDU header is placed here before receive_callback() is 
	     * called by boost::asio.
	     *
	     * Since the AgentX-header is always 20 bytes in length, this 
	     * buffer is 20 bytes in size.
	     */
	    // TODO: avoid magic numbers, even if they are documented.
	    byte_t header_buf[20];

	    /**
	     * \brief The handler object for incoming %PDU's.
	     *
	     * This handler object is informed by receive_callback() for each 
	     * received PDU (except ResponsePDU's).
	     *
	     * The pointer may be null, which means that there is no handler 
	     * object registered.
	     */
	    pdu_handler* handler;

	    /**
	     * \brief Hide standard constructor.
	     *
	     * We need an io_service object to function properly.
	     */
	    connector();
	    
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
	    connector(boost::shared_ptr<boost::asio::io_service> io_service,
		       const std::string& unix_domain_socket,
		       unsigned timeout);

	    /**
	     * \brief Wait with timeout for a reponse.
	     *
	     * This function blocks until a ResponsePDU with the given 
	     * packetID is received or until the timeout expires, whichever 
	     * comes first.  The received ResponsePDU (if any) is returned.
	     *
	     * This function calls run_one() repeatedly on the io_service 
	     * object until the desired ResponsePDU arrives or the timeout 
	     * expires.  This may cause other asynchronous operations to be 
	     * served, as well.
	     *
	     * \param packetID The packetID to wait for.
	     *
	     * \exception timeout_exception If the timeout expired before the
	     *                              ResponsePDU was received. The 
	     *                              connector object stays in 
	     *                              connected state.
	     *
	     * \exception disconnected If disconnected.  This is also thrown if
	     *                         the operation fails and the object gets 
	     *                         disconnected for that reason.
	     *
	     * \return The received ResponsePDU.
	     */
	    boost::shared_ptr<ResponsePDU>
		wait_for_response(uint32_t packetID);


	    /**
	     * \brief Register a handler object for received %PDU's.
	     *
	     * Every time a %PDU is received, the handler object's handle_pdu() 
	     * method will be invoked with the %PDU as argument. This method is 
	     * executed in the context of the io_service object's run() or 
	     * run_one() method.  Care should be taken to not block the call, 
	     * e.g. by doing networking.
	     *
	     * After registering a handler it can be unregistered again by 
	     * calling this function with a null pointer.  While no handler 
	     * object is registered, received %PDU's are silently discarded.
	     *
	     * \note Any exceptions thrown by the handler object are silently
	     *       discarded.
	     *
	     * \note There can be only one handler object registered at a time.
	     *
	     * \param handler A pointer to the handler object, or null.
	     *
	     * \exception None.
	     */
	    void register_handler( pdu_handler* );

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

	    /**
	     * \brief Find out whether the object is currently connected.
	     *
	     * \return Whether the object is in state connected.
	     *
	     * \exception None.
	     */
	    bool is_connected()
	    {
		return (this->socket != 0 ? true : false );
	    }

	    /**
	     * \brief send a %PDU to the remote entity.
	     *
	     * The %PDU is sent to the remote entity. If the timeout expires 
	     * during the send operation, a timeout_error is thrown and the 
	     * object gets disconnected.
	     *
	     * \note The run_one() of the io_service object is called at least
	     *       one time by this operation.
	     *
	     * \param pdu The PDU to send
	     *
	     * \exception timeout_error If a timeout error occurs. The function
	     *                          uses the timeout value given during 
	     *                          construction. The objects gets 
	     *                          disconnected in this case. Some data 
	     *                          might be sent already.
	     *
	     * \exception disconnected If disconnected. This is also thrown if
	     *                         sending fails and the object gets 
	     *                         disconnected for that reason. Some data 
	     *                         might be sent already.
	     */
	    void send(const PDU& pdu);

	    /**
	     * \brief Destructor
	     *
	     * \exception None.
	     */
	    ~connector()
	    {
		// Disconnect if applicable. This also destroys the socket (if 
		// any).
		disconnect();
	    }

    };

}

#endif

