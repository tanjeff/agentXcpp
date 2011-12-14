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
#include "connection.hpp"
#include "helper.hpp"


using namespace agentxcpp;



/*
 ********************************************
  Local helper function: read_with_timeout()
 ********************************************
 */



/**
 * \brief Helper type for read_with_timeout(). See there for an explanation.
 */
enum status_t {
    in_progress = 0,
    success     = 1,
    fail        = 2
};



/**
 * \brief Helper function for read_with_timeout(). See there for an
 *        explanation.
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
 * May throw the same exceptions as boost::asio::read().
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
 * \return How many bytes were read
 */

//void async_read(
//    AsyncReadStream & s,
//    const MutableBufferSequence & buffers,
//    ReadHandler handler);

template<typename AsyncReadStream,
         typename MutableBufferSequence>
static void read_with_timeout(AsyncReadStream& s,
			      const MutableBufferSequence& buffers,
			      unsigned int timeout)
{

    // Start timeout timer
    boost::asio::deadline_timer timer(s.get_io_service());
    status_t timer_result = in_progress;
    timer.expires_from_now( boost::posix_time::milliseconds(timeout) );
    timer.async_wait( boost::bind(callback,
				  boost::asio::placeholders::error,
				  &timer_result) );

    // Start read
    status_t read_result = in_progress;
    async_read(s,
	       buffers,
	       boost::bind(callback,
			   boost::asio::placeholders::error,
			   &read_result));

    // process asio events until read succeeds or timeout expires
    do
    {
	s.get_io_service().run_one();
    }
    while(read_result == in_progress && timer_result == in_progress);

    // Check read result
    switch(read_result)
    {
	case success:
	    // Read succeeded: OK
	    timer.cancel();
	    return;

	case fail:
	    // read failed: cancel timer, throw exception
	    timer.cancel();
	    throw( network_error() );

	case in_progress:

	    // Look at timer_result:
	    switch(timer_result)
	    {
		case success:
		    // timer fired while reading
		    // cancel read, throw exception
		    s.cancel();
		    throw( timeout_error() );
		case fail:
		    // timer failed while reading
		    // what now?
		    // I think we should fail with a network error
		    s.cancel();
		    throw( network_error() );
		case in_progress:
		    // It didn't happen -> ignore
		    break;
	    }
    }

}



/*
 ********************************************
  Implentation of class master_proxy
 ********************************************
 */
	    

connection::connection(boost::asio::io_service* io_service,
		       const std::string& unix_domain_socket,
		       unsigned timeout) :
    timeout(timeout),
    io_service(io_service),
    socket(*io_service),
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
	// Ignore any errors (don't throw anything)
    }
}

void connection::connect()
{
    if( this->socket.is_open() )
    {
	// we are already connected -> nothing to do
	return;
    }

    // Connect to endpoint
    try
    {
	socket.connect(endpoint);
    }
    catch(boost::system::system_error)
    {
	// Could not connect
	throw(disconnected());
	return;
    }
}

void connection::disconnect()
{
    if( ! this->socket.is_open() )
    {
	// we are already disconnected -> nothing to do
	return;
    }
    
    // Close socket
    try
    {
	socket.close();
    }
    catch(...)
    {
	// ignore errors
    }
}




void connection::register_handler( void (*handler)(shared_ptr<PDU>) )
{
    this->handler = handler;
}


void connection::receive_callback(const boost::system::error_code& result)
{
    // Check for network errors
    if( result.value() != 0 )
    {
	// async read operation failed
	// -> close socket (without notifying the master agent)
	this->disconnect();

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
	read_with_timeout(this->socket,
			  boost::asio::buffer(payload, payload_length),
			  this->timeout);
    }
    catch(timeout_error)
    {
	// Reading payload timed out
	// -> abort connection
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
	// We cannot handle this PDU.
	// -> ignore
    }
    catch(parse_error)
    {
	// Close socket
	this->disconnect();
    }

    // Call the handler
    if( this->handler )
    {
	this->handler(pdu);
    }

    // Set up socket for next read access
    async_read(this->socket,
	       boost::asio::buffer(this->header_buf, 20),
	       boost::bind(&connection::receive_callback,
			   this,
			   boost::asio::placeholders::error));
}

