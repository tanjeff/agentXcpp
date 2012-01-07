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

#include <boost/bind.hpp>
#include "connector.hpp"
#include "helper.hpp"


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
    in_progress = 0,
    success     = 1,
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
	*retval = success;
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
			      unsigned int timeout)
{
    //
    // What this function does:
    //
    // 1) start timeout timer
    // 2) start read
    // 3) wait until timer or read completes
    // 4) process result
    //

    // The timer_result and read_result variables are static because in some 
    // circumstances the callback (which manipulates them) might be called 
    // after this function returned. We avoid a segfault this way.
    static status_t timer_result;
    static status_t read_result;
    
    // 1) Start timeout timer
    boost::asio::deadline_timer timer(s.get_io_service());
    timer_result = in_progress;
    try
    {
	// throws system_error in boost 1.45.0
	timer.expires_from_now( boost::posix_time::milliseconds(timeout) );
    }
    catch(boost::system::system_error)
    {
	throw( network_error() );
    }
    // doesn't throw in boost 1.45.0:
    timer.async_wait( boost::bind(callback,
				  boost::asio::placeholders::error,
				  &timer_result) );

    // 2) Start read
    read_result = in_progress;
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
	while(read_result == in_progress && timer_result == in_progress);
    }
    catch(boost::system::system_error)
    {
	try
	{
	    // throws system_error in boost 1.45.0
	    timer.cancel();

	    // TODO: How to cancel the async_read operation?
	}
	catch(boost::system::system_error)
	{
	    // Is the timer uncancelled now? Will it possibly fire our 
	    // callback? On the other hand, leaving this function will 
	    // destroy the deadline_timer object anyway.

	    // -> ignore
	}
	throw( network_error() );
    }

    // 4) Check read result
    switch(read_result)
    {
	case success:
	    // Read succeeded: OK
	    try
	    {
		// throws system_error in bost 1.45.0:
		timer.cancel();
	    }
	    catch(boost::system::system_error)
	    {
		// Is the timer uncancelled now? Will it possibly fire our 
		// callback? On the other hand, leaving this function will 
		// destroy the deadline_timer object anyway.

		// -> ignore
	    }
	    return;

	case fail:
	    // read failed: cancel timer, throw exception
	    try
	    {
		// throws system_error in bost 1.45.0:
		timer.cancel();
	    }
	    catch(boost::system::system_error)
	    {
		// Is the timer uncancelled now? Will it possibly fire our 
		// callback? On the other hand, leaving this function will 
		// destroy the deadline_timer object anyway.

		// -> ignore
	    }
	    throw( network_error() );

	case in_progress:

	    // Look at timer_result:
	    switch(timer_result)
	    {
		case success:
		    // timer fired while reading
		    
		    // TODO: how to cancel the async read operation?
		    
		    throw( timeout_error() );

		case fail:
		    // timer failed while reading
		    // what now?
		    // I think we should fail with a network error
		    
		    // TODO: how to cancel the async read operation?
		    
		    throw( network_error() );

		case in_progress:
		    // It didn't happen -> ignore
		    break;
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
    
    // The timer_result and send_result variables are static because in some 
    // circumstances the callback (which manipulates them) might be called 
    // after this function returned. We avoid a segfault this way.
    static status_t timer_result;
    static status_t send_result;


    // 1) Start timeout timer
    
    // doesn't throw in boost 1.45.0:
    boost::asio::deadline_timer timer(s.get_io_service());
    timer_result = in_progress;
    try
    {
	// throws system_error in boost 1.45.0
	timer.expires_from_now( boost::posix_time::milliseconds(timeout) );
    }
    catch(boost::system::system_error)
    {
	throw( network_error() );
    }
    // doesn't throw in boost 1.45.0:
    timer.async_wait( boost::bind(callback,
				  boost::asio::placeholders::error,
				  &timer_result) );

    // 2) Start send
    send_result = in_progress;
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
	while(send_result == in_progress && timer_result == in_progress);
    }
    catch(boost::system::system_error)
    {
	try
	{
	    // throws system_error in boost 1.45.0
	    timer.cancel();

	    // TODO: How to cancel the async_send operation?
	}
	catch(boost::system::system_error)
	{
	    // Is the timer uncancelled now? Will it possibly fire our 
	    // callback? On the other hand, leaving this function will 
	    // destroy the deadline_timer object anyway.

	    // -> ignore
	}
	throw( network_error() );
    }

    // 4) Check result
    switch(send_result)
    {
	case success:
	    // Send succeeded:
	    try
	    {
		// throws system_error in bost 1.45.0:
		timer.cancel();
	    }
	    catch(boost::system::system_error)
	    {
		// Is the timer uncancelled now? Will it possibly fire our 
		// callback? On the other hand, leaving this function will 
		// destroy the deadline_timer object anyway.

		// -> ignore
	    }
	    return;

	case fail:
	    // send failed: cancel timer, throw exception
	    try
	    {
		// throws system_error in bost 1.45.0:
		timer.cancel();
	    }
	    catch(boost::system::system_error)
	    {
		// Is the timer uncancelled now? Will it possibly fire our 
		// callback? On the other hand, leaving this function will 
		// destroy the deadline_timer object anyway.

		// -> ignore
	    }
	    throw( network_error() );

	case in_progress:

	    // Sending still in progress, look at timer_result:
	    switch(timer_result)
	    {
		case success:
		    // timer fired while reading
		    
		    // TODO: how to cancel the async send operation?

		    // throw exception
		    throw( timeout_error() );

		case fail:
		    // timer failed while sending
		    // what now?
		    // I think we should fail with a network error
		    
		    // TODO: how to cancel the async send operation?
		    
		    throw( network_error() );

		case in_progress:
		    // It didn't happen -> ignore
		    break;
	    }
    }
}



/*
 ********************************************
  Implentation of class connector
 ********************************************
 */
	    

connector::connector(boost::shared_ptr<boost::asio::io_service> io_service,
		       const std::string& unix_domain_socket,
		       unsigned timeout) :
    timeout(timeout),
    io_service(io_service),
    socket(0),
    endpoint(unix_domain_socket.c_str()),
    handler(0)
{
    // Try to start connected
    try
    {
	connect();
    }
    catch(...)
    {
	// Ignore any errors (don't throw)
    }
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




void connector::register_handler( void (*handler)(shared_ptr<PDU>) )
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
    data_t buf;
    buf.append(this->header_buf, 20);

    // read endianess flag
    bool big_endian = ( this->header_buf[2] & (1<<4) ) ? true : false;

    // read payload length
    uint32_t payload_length;
    data_t::const_iterator pos = buf.begin() + 16;
    payload_length = read32(pos, big_endian);
    if( payload_length % 4 != 0 )
    {
	// payload length must be a multiple of 4!
	// See RFC 2741, 6.1. "AgentX PDU Header"
	// -> close socket
	this->disconnect();
    }

    // Read the payload (TODO: can we avoid the new() operator?)
    byte_t* payload = new byte_t[payload_length];
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

    // Further processing only if handler is registered.
    if( this->handler )
    {
	// Parse PDU
	shared_ptr<PDU> pdu;
	try
	{
	    pdu = PDU::parse_pdu(buf);
	}
	catch(version_error)
	{
	    // We cannot handle this PDU.
	    // -> ignore
	}
	catch(parse_error)
	{
	    // disconnect
	    this->disconnect();
	}

	// Call the handler
	try
	{
	    this->handler(pdu);
	}
	catch(...)
	{
	    // discard exceptions from user handler
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
    try
    {
	// throws timeout_error and network_error:
	data_t buf = pdu.serialize();
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



// Helper function for master_proxy::wait_for_response(). See there for an 
// explanation.
static void callback_for_response(const boost::system::error_code& result,
                                 status_t* retval)
{
    if( result.value() == 0 )
    {
	// success
	*retval = success;
    }
    else
    {
	// error
	*retval = fail;
    }
}
/*
 * Timeout handling:
 * 
 * The timeout handling is realized using a boost::asio::deadline_timer 
 * object, which is set up to call the callback_for_response() function when 
 * it expires (or when it fails). The variable 'timer_result' is intially set 
 * to 'in_progress'.  The callback will set it to 'success' or 'fail' when the 
 * timer expires respectively fails.
 *
 * Then the io_service.run_one() function is invoked repeatedly until either 
 * the timer expired or the desired ResponsePDU arrived. Note that 
 * io_service.run_one() may service other asynchronous operations first, e.g.  
 * a get request.
 *
 * Finally either the received ResponsePDU is returned or a timeout exception 
 * is thrown.
 */
boost::shared_ptr<ResponsePDU>
connector::wait_for_response(uint32_t packetID)
{
    // Indicate that we are waiting for a specific response:
    // We add a null pointer to the map
    responses[packetID] = boost::shared_ptr<ResponsePDU>();

    // Start timeout timer
    boost::asio::deadline_timer timer(*(this->io_service));
    status_t timer_result = in_progress; // callback stores result here
    timer.expires_from_now( boost::posix_time::seconds(this->timeout) );
    timer.async_wait( boost::bind(callback_for_response,
				  boost::asio::placeholders::error,
				  &timer_result) );

    // process asio events until ResponsePDU arrives or timeout expires
    try
    {
	while(   this->responses[packetID].get() == 0 // no response was stored
		 && timer_result == in_progress)
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
	timer.cancel();
	shared_ptr<ResponsePDU> retval = this->responses[packetID];
	this->responses.erase( packetID );
	return retval;
    }
    else
    {
	// Timer expired or failed before ResponsePDU arrived
	throw(timeout_error());
    }
}
