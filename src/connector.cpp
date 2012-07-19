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

#include <boost/bind.hpp>
#include "connector.hpp"
#include "util.hpp"
#include "timeout_timer.hpp"


using namespace agentxcpp;


/*
 ********************************************
  Local helper function: read_with_timeout()
 ********************************************
 */



/**
 * \brief Helper type for *_with_timeout() functions.
 */
enum status_t {
    in_progress_old = 0,
    success_old     = 1,
    fail        = 2
};



/**
 * \brief Helper function for *_with_timeout() functions.
 */
static void callback(const boost::system::error_code& result,
		     status_t* retval)
{
    if( result.value() == 0 )
    {
        // success
	*retval = success_old;
    }
    else
    {
	// error
	*retval = fail;
    }
}




/**
 * \brief Like boost::asio::read(), but with timeout
 *
 * Calls boost::asio::read(), but provides a timeout in addition.
 *
 * This function calls s.get_io_service.run_one().
 *
 * \param s The Stream to read from
 *
 * \param buffers The buffers to read into
 *
 * \param timeout The desired timeout in milliseconds
 *
 * \exception timeout_exception If the timeout expires before the read
 *                              operation completes. Some bytes may have been 
 *                              read.
 *
 * \exception network_error If reading failed. Some data may have been read
 *                          already or will be read later. Subsequent calls to 
 *                          this function will lead to undefined results.
 *
 * \return How many bytes were read
 */
template<typename AsyncReadStream,
         typename MutableBufferSequence>
static void read_with_timeout(AsyncReadStream& s,
			      const MutableBufferSequence& buffers,
			      unsigned timeout)
{
    //
    // What this function does:
    //
    // 1) start timeout timer
    // 2) start read
    // 3) wait until timer or read completes
    // 4) process result
    //

    // 1) Start timeout timer
    timeout_timer timer(s.get_io_service());
    timer.expires_from_now( boost::posix_time::milliseconds(timeout) );

    // 2) Start read
    
    // The read_result variable is static because in some circumstances the 
    // callback (which manipulates it) might be called after this function 
    // returned. We avoid a segfault this way.
    static status_t read_result = in_progress_old;
    // doesn't throw in boost 1.45.0:
    async_read(s,
	       buffers,
	       boost::bind(callback,
			   boost::asio::placeholders::error,
			   &read_result));

    // 3) process asio events until read succeeds or timeout expires
    try
    {
	do
	{
	    // throws system_error in boost 1.45.0:
	    s.get_io_service().run_one();
	}
	while(read_result == in_progress_old &&
              timer.get_status() == timeout_timer::running);
    }
    catch(boost::system::system_error)
    {
        timer.cancel();

        // TODO: How to cancel the async_read operation?
        throw( network_error() );
    }

    // 4) Check read result
    switch(read_result)
    {
	case success_old:
	    // Read succeeded: OK
            timer.cancel();
	    return;

	case fail:
	    // read failed: cancel timer, throw exception
            timer.cancel();
	    throw( network_error() );

	case in_progress_old:

	    // Look at timer_result:
	    switch(timer.get_status())
	    {
		case timeout_timer::expired:
		    // timer fired while reading
		    
		    // TODO: how to cancel the async read operation?
		    
		    throw( timeout_error() );

                default:
                    // Timer broke or reported an insane status
                    // -> fail with a network error

                    // TODO: how to cancel the async read operation?

		    throw( network_error() );
	    }
    }

}



/**
 * \brief Send some data with timeout
 *
 * \note This function calls s.get_io_service.run_one().
 *
 * \param s The Stream to send to to
 *
 * \param buffers The data to send
 *
 * \param timeout The desired timeout in milliseconds
 *
 * \exception timeout_error If the timeout expires before the send
 *                          operation completes. Some data may have been sent 
 *                          already. Subsequent calls to this function will 
 *                          lead to undefined results.
 *
 * \exception network_error If sending failed. Some data may have been sent
 *                          already or may still be in the send queue.  
 *                          Subsequent calls to this function will lead to 
 *                          undefined results.
 */
template<typename ConstBufferSequence>
static void send_with_timeout(boost::asio::local::stream_protocol::socket& s,
			      const ConstBufferSequence& buffers,
			      unsigned timeout)
{
    //
    // What this function does:
    //
    // 1) start timeout timer
    // 2) start send
    // 3) wait until timer or send completes
    // 4) process result
    //
    
    // 1) Start timeout timer
    
    timeout_timer timer(s.get_io_service());
    timer.expires_from_now( boost::posix_time::milliseconds(timeout) );
    
    // 2) Start send
    
    // The send_result variable is static because in some circumstances the 
    // callback (which manipulates it) might be called after this function 
    // returned. We avoid a segfault this way.
    static status_t send_result = in_progress_old;

    // doesn't throw in boost 1.45.0:
    s.async_send(buffers,
		 boost::bind(callback,
			     boost::asio::placeholders::error,
			     &send_result));

    // 3) process asio events until send succeeds or timeout expires
    
    try
    {
	do
	{
	    // throws system_error in boost 1.45.0:
	    s.get_io_service().run_one();
	}
	while(send_result == in_progress_old &&
              timer.get_status() == timeout_timer::running);
    }
    catch(boost::system::system_error)
    {
        timer.cancel();

        // TODO: How to cancel the async_send operation?
        throw( network_error() );
    }

    // 4) Check result

    switch(send_result)
    {
	case success_old:
	    // Send succeeded:
            timer.cancel();
	    return;

	case fail:
	    // send failed: cancel timer, throw exception
            timer.cancel();
	    throw( network_error() );

	case in_progress_old:
            // Sending still in progress, look at timer_result:
	    switch(timer.get_status())
	    {
                case timeout_timer::expired:
                    // timer fired while reading
		    
		    // TODO: how to cancel the async send operation?

		    // throw exception
		    throw( timeout_error() );

		default:
                    // Timer broke or reported an insane status
                    // -> fail with a network error
		    
		    // TODO: how to cancel the async send operation?
		    
		    throw( network_error() );
	    }
    }
}



/*
 ********************************************
  Implentation of class connector
 ********************************************
 */





connector::connector(boost::asio::io_service* io_service,
		       const std::string& unix_domain_socket,
		       unsigned timeout) :
    timeout(timeout),
    io_service(io_service),
    socket(0),
    endpoint(unix_domain_socket.c_str()),
    handler(0)
{
}

void connector::connect()
{
    // If currently connected: do nothing
    // (is_open() doesn't throw in boost 1.45.0)
    if( this->socket )
    {
	return;
    }

    // Connect to endpoint
    using boost::asio::local::stream_protocol;
    this->socket = new stream_protocol::socket(*io_service);
    try
    {
	this->socket->connect(endpoint);
    }
    catch(boost::system::system_error)
    {
	// Could not connect
	delete this->socket;
	this->socket = 0;
	throw(disconnected());
    }
    
    // Set up socket for next read access
    // (async_read doesn't throw in boost 1.45.0)
    async_read(*this->socket,
	       boost::asio::buffer(this->header_buf, 20),
	       boost::bind(&connector::receive_callback,
			   this,
			   boost::asio::placeholders::error));
}

void connector::disconnect()
{
    // If already disconnected: do nothing
    if( this->socket == 0 )
    {
	return;
    }
    
    // Shutdown socket
    try
    {

	// Cancel all read and write operations. Called on the recommendation 
	// of socket.close() documentation.
	// (throws system_error in boost 1.45.0)
	socket->shutdown(
		boost::asio::local::stream_protocol::socket::shutdown_both);
    }
    catch(boost::system::system_error)
    {
	// ignore errors
    }

    // Close socket
    try
    {
	// (throws system_error in boost 1.45.0)
	socket->close();
    }
    catch(boost::system::system_error)
    {
	// ignore errors
    }
    
    // Finally destroy socket
    delete this->socket;
    this->socket = 0;
}




void connector::register_handler( pdu_handler *handler )
{
    this->handler = handler;
}


void connector::receive_callback(const boost::system::error_code& result)
{
    // Check for network errors
    if( result.value() != 0 )
    {
	// result will probably be boost::asio::error::operation_aborted if the 
	// socket is explicitly closed. See boost 1.45.0 docs for 
	// basic_stream_socket::close().
	if( result.value() == boost::asio::error::operation_aborted )
	{
	    // Socket was closed. Nothing to do here.
	}
	else
	{
	    // async read operation failed
	    // -> disconnect
	    this->disconnect();
	}

	// Nothing left to do
	return;
    }

    // Copy header into PDU buffer
    binary buf;
    buf.append(this->header_buf, 20);

    // read endianess flag
    bool big_endian = ( this->header_buf[2] & (1<<4) ) ? true : false;

    // read payload length
    uint32_t payload_length;
    binary::const_iterator pos = buf.begin() + 16;
    payload_length = read32(pos, big_endian);
    if( payload_length % 4 != 0 )
    {
	// payload length must be a multiple of 4!
	// See RFC 2741, 6.1. "AgentX PDU Header"
	// -> close socket
	this->disconnect();

	// Report parse error
	if( this->handler )
	{
	    try
	    {
		this->handler->handle_pdu(shared_ptr<PDU>(), -1);
	    }
	    catch(...)
	    {
		// discard exceptions from user handler
	    }
	}
    }

    // Read the payload (TODO: can we avoid the new() operator?)
    uint8_t* payload = new uint8_t[payload_length];
    try
    {
	read_with_timeout(*this->socket,
			  boost::asio::buffer(payload, payload_length),
			  this->timeout);
    }
    catch(...)
    {
	// Some error occurred, e.g. timeout
	// -> disconnect
	this->disconnect();
	delete[] payload;
	return;
    }
    buf.append(payload, payload_length);
    delete[] payload;
	
    // Parse PDU
    shared_ptr<PDU> pdu;
    try
    {
	pdu = PDU::parse_pdu(buf);
    }
    catch(version_error)
    {
	// Report version error
	if( this->handler )
	{
	    try
	    {
		this->handler->handle_pdu(shared_ptr<PDU>(), -2);
	    }
	    catch(...)
	    {
		// discard exceptions from user handler
	    }
	}
    }
    catch(parse_error)
    {
	// disconnect
	this->disconnect();
	
	// Report parse error
	if( this->handler )
	{
	    try
	    {
		this->handler->handle_pdu(shared_ptr<PDU>(), -1);
	    }
	    catch(...)
	    {
		// discard exceptions from user handler
	    }
	}
    }

    // Special case: ResponsePDU's
    shared_ptr<ResponsePDU> response;
    response = boost::dynamic_pointer_cast<ResponsePDU>(pdu);
    if(response.get() != 0)
    {
	// Was a response
	std::map< uint32_t, boost::shared_ptr<ResponsePDU> >::iterator i;
	i = this->responses.find( response->get_packetID() );
	if(i != this->responses.end())
	{
	    // Someone is waiting for this response
	    i->second = response;
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
	// -> call handler if available
	if( this->handler )
	{

	    // Call the handler
	    try
	    {
		this->handler->handle_pdu(pdu, 0);
	    }
	    catch(...)
	    {
		// discard exceptions from user handler
	    }
	}
    }

    // Set up socket for next read access
    // (async_read doesn't throw in boost 1.45.0)
    async_read(*this->socket,
	       boost::asio::buffer(this->header_buf, 20),
	       boost::bind(&connector::receive_callback,
			   this,
			   boost::asio::placeholders::error));
}



void connector::send(const PDU& pdu)
{
    if(this->socket == 0)
    {
	// We are currently not conected
	throw disconnected();
    }

    try
    {
	// throws timeout_error and network_error:
	binary buf = pdu.serialize();
	send_with_timeout(*this->socket,
			  boost::asio::buffer(buf.data(), buf.size()),
			  this->timeout);
    }
    catch(network_error)
    {
	// network errors are fatal: disconnect & throw
	this->disconnect();
	throw disconnected();
    }
    catch(timeout_error)
    {
	// No subsequent calls to send_with_timeout() possible -> disconnect.
	this->disconnect();
	
	// forward timeout_error to caller
	throw;
    }
}



boost::shared_ptr<ResponsePDU>
connector::wait_for_response(uint32_t packetID)
{
    if(this->socket == 0)
    {
	// We are currently not conected
	throw disconnected();
    }

    // Indicate that we are waiting for a specific response:
    // We add a null pointer to the map
    responses[packetID] = boost::shared_ptr<ResponsePDU>();

    // Start timeout timer
    timeout_timer timer(*(this->io_service));
    timer.expires_from_now( boost::posix_time::seconds(this->timeout) );

    // process asio events until ResponsePDU arrives or timeout expires
    try
    {
	while(   this->responses[packetID].get() == 0 // no response was stored
		 && timer.get_status() == timeout_timer::running) // no timeout yet
	{
	    this->io_service->run_one();
	}
    }
    catch(boost::system::system_error)
    {
	// run_one() failed
	// -> disconnect and fail
	this->disconnect();
	throw disconnected();
    }

    // Check the result
    if( this->responses[packetID].get() != 0 )
    {
	// ResponsePDU arrived:
	// 1. Cancel timer
	// 2. Erase response from map
	// 3. Return response
	timer.cancel(); // does nothing if timer broke
	shared_ptr<ResponsePDU> retval = this->responses[packetID];
	this->responses.erase( packetID );
	return retval;
    }
    else
    {
        switch(timer.get_status())
        {
            case timeout_timer::expired:
                // Timer expired before ResponsePDU arrived
                throw(timeout_error());
            default:
                // Timer broke or reported an insane status
                // We go to disconnected state to indicate an error
                this->disconnect();
                throw disconnected();
        }
    }
}
