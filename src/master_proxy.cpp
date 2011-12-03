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

#include "master_proxy.hpp"
#include "OpenPDU.hpp"
#include "ClosePDU.hpp"
#include "ResponsePDU.hpp"
#include "RegisterPDU.hpp"
#include "GetPDU.hpp"
#include "helper.hpp"
#include "types.hpp"

#include <iostream>
using namespace std;

using namespace agentxcpp;
using namespace boost;
using boost::shared_ptr;



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
	    throw( network() );

	case in_progress:

	    // Look at timer_result:
	    switch(timer_result)
	    {
		case success:
		    // timer fired while reading
		    // cancel read, throw exception
		    s.cancel();
		    throw( timeout_exception() );
		case fail:
		    // timer failed while reading
		    // what now?
		    // I think we should fail with a network error
		    s.cancel();
		    throw( network() );
		case in_progress:
		    // What the fuck?
		    // How was it possible to leave the loop above as long as 
		    // both async operations where in progress ?!
		    // This simply can't happen -> ignore
		    break;
	    }
    }

}




/*
 ********************************************
  Implentation of class master_proxy
 ********************************************
 */



master_proxy::master_proxy(boost::asio::io_service* _io_service,
			   std::string _description,
			   byte_t _default_timeout,
			   oid _id,
			   std::string _filename) :
    io_service(_io_service),
    io_service_by_user(true),
    socket(*io_service),
    endpoint(_filename.c_str()),
    description(_description),
    default_timeout(_default_timeout),
    id(_id)
{
    // Try to connect
    connect();
}



master_proxy::master_proxy(std::string _description,
			   byte_t _default_timeout,
			   oid _id,
			   std::string _filename) :
    io_service(new boost::asio::io_service()),
    io_service_by_user(false),
    socket(*io_service),
    endpoint(_filename.c_str()),
    description(_description),
    default_timeout(_default_timeout),
    id(_id)
{
    // Try to connect
    connect();
}


void master_proxy::connect()
{
    if( this->is_connected() )
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
	// Could not connect -> close the socket
	kill_connection();
	return;
    }

    // Send OpenPDU
    OpenPDU openpdu;
    openpdu.set_timeout(default_timeout);
    openpdu.set_id(id);
    data_t buf = openpdu.serialize();
    socket.send(asio::buffer(buf.data(), buf.size()));

    // Start receiving PDU's asynchronously (the callback will call 
    // async_read() to continue receiving)
    async_read(this->socket,
	       boost::asio::buffer(this->header_buf, 20),
	       boost::bind(&master_proxy::receive,
			   this,
			   boost::asio::placeholders::error));

    // Wait for response
    boost::shared_ptr<ResponsePDU> response;
    try
    {
	response = wait_for_response(openpdu.get_packetID());
    }
    catch(...)
    {
	// Something went wrong -> close socket
	kill_connection();
	return;
    }

    if(response->get_error() != ResponsePDU::noAgentXError)
    {
	// Some error occured, disconnect
	kill_connection();
	return;
    }

    this->sessionID = response->get_sessionID();
}




void master_proxy::disconnect(ClosePDU::reason_t reason)
{
    if( !this->is_connected() )
    {
	// we are already disconnected -> nothing to do
	return;
    }

    // According to RFC 2741, 7.1.8. "Processing the agentx-Close-PDU", the 
    // master agent unregisters all MIB regions, frees all index values and all 
    // sysORID are removed. Thus no need to clean up before ClosePDU is sent.

    // Send ClosePDU
    ClosePDU pdu(sessionID, reason);
    data_t buf = pdu.serialize();
    socket.send(asio::buffer(buf.c_str(), buf.size()));

    // Wait for response
    ResponsePDU* response;
    try
    {
	response = dynamic_cast<ResponsePDU*>(PDU::get_pdu(socket));
    }
    catch(...)
    {
	// ignore errors from PDU::get_pdu()
    }

    // Check for errors
    if(response != 0
       && response->get_error() != ResponsePDU::noAgentXError)
    {
	// Some error occured, disconnect
	kill_connection();
	return;
    }

    // Close socket
    socket.close();
}

master_proxy::~master_proxy()
{
    // Disconnect from master agent
    disconnect(ClosePDU::reasonShutdown);

    // Destroy io_service object if needed
    if( ! this->io_service_by_user )
    {
	delete this->io_service;
    }
}

void master_proxy::register_subtree(oid subtree,
				    byte_t priority,
				    byte_t timeout)
{
    // Are we connected?
    if( ! is_connected())
    {
	throw(disconnected());
    }

    // Build up the RegisterPDU
    RegisterPDU pdu;
    pdu.set_subtree(subtree);
    pdu.set_priority(priority);
    pdu.set_timeout(timeout);
    pdu.set_sessionID(this->sessionID);

    // Send RegisterPDU
    data_t buf = pdu.serialize();
    socket.send(asio::buffer(buf.c_str(), buf.size()));

    // Wait for response
    boost::shared_ptr<ResponsePDU> response;
    try{
	response = wait_for_response(pdu.get_packetID());
    }
    catch(timeout_exception e)
    {
	// Throw timeout exception
	throw(e);
    }

    // Check Response
    switch(response->get_error())
    {
	// General errors:

	case ResponsePDU::parseError:
	    // Oops, we sent a malformed PDU to the master
	    throw(internal_error());

	case ResponsePDU::notOpen:
	    // We checked the connection state before, but maybe we lost the 
	    // connection during communication...
	    throw(disconnected());

	case ResponsePDU::unsupportedContext:
	    // We do currently not really support contexts in this library. An 
	    // invalid context is thus probably an agentxcpp bug.
	    throw(internal_error());

	case ResponsePDU::processingError:
	    // master was unable to process the request
	    throw(master_is_unable());

	case ResponsePDU::noAgentXError:
	    // Hey, it worked! Nothing left to do here, continuing...
	    break;

	// Register-specific errors:

	case ResponsePDU::duplicateRegistration:
	    throw(duplicate_registration());

	case ResponsePDU::requestDenied:
	    throw(master_is_unwilling());

	default:
	    // This is a cae of can-not-happen. Probably the master is buggy. 
	    // The agentxcpp library is bug-free of course ;-)
	    // We throw a parse error meanwhile, because we didn't expect the 
	    // response to look like that...
	    throw(parse_error());
    }

    // Finish
    return;
}

void master_proxy::receive(const boost::system::error_code& result)
{
    // Check for network errors
    if( result.value() != 0 )
    {
	// async read operation failed
	// -> close socket (without notifying the master agent)
	kill_connection();

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
	kill_connection();
    }

    // Read the payload (TODO: can we avoid the new() operator?)
    byte_t* payload = new byte_t[payload_length];
    boost::asio::read(this->socket,
		      boost::asio::buffer(payload, payload_length));
    buf.append(payload, payload_length);
    delete[] payload;

    // Parse PDU
    shared_ptr<PDU> pdu;
    bool dispatch_pdu = true; // some PDUs are discarded
    try
    {
	pdu = PDU::parse_pdu(buf);
    }
    catch(version_mismatch)
    {
	// discard PDU with wrong version
	dispatch_pdu = false;
    }
    catch(parse_error)
    {
	// Close socket
	kill_connection();
    }

    // Dispatch!
    if( dispatch_pdu )
    {
	// We use dynamic_cast to find out which type of PDU we received.
	
	if(shared_ptr<ResponsePDU> r = boost::dynamic_pointer_cast<ResponsePDU>(pdu))
	{
	    // Was a ResponsePDU: is someone waiting for it?
	    // Hint: a waited-for response is indicated by putting a null 
	    // pointer into the 'responses' map.
	    if(this->responses.find(r->get_packetID()) != this->responses.end()
	       && this->responses[r->get_packetID()].get() == 0)
	    {
		// someone waits for this PDU: give it to him!
		this->responses[r->get_packetID()] = r;
	    }
	    else
	    {
		// Either nobode waits for it or another response with the 
		// same packetID is still stored within the 'responses' map.  
		// In either case, we deiscard the message.
		// (Leaving this block destroys it)
	    }
	}
	else if(shared_ptr<GetPDU> get = boost::dynamic_pointer_cast<GetPDU>(pdu))
	{
	    // was a GtPDU: for now, we simply put something on the console.
	    cout << "Received Get-PDU:" << endl;
	    vector<oid> search_range = get->get_sr();
	    vector<oid>::const_iterator i;
	    for(i=search_range.begin(); i != search_range.end(); i++)
	    {
		cout << "    " << *i << endl;
	    }
	}
	else
	{
	    // We don't like this type of PDU.
	    // Ignoring... but put something on the console
	    cout << "Request received from the master. Leaving it unanalyzed..." << endl;
	}
    }

    // Set up socket for next read access
    async_read(this->socket,
	       boost::asio::buffer(this->header_buf, 20),
	       boost::bind(&master_proxy::receive,
			   this,
			   boost::asio::placeholders::error));
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
shared_ptr<ResponsePDU> master_proxy::wait_for_response(uint32_t packetID,
							unsigned timeout)
{
    // Handle omitting timeout parameter
    if( timeout == 0 )
    {
	if(this->default_timeout == 0)
	{
	    // No timeout given: fall back to 1 second
	    timeout = 1000;
	}
	else
	{
	    // No timeout given to function, but we have a session timeout: 
	    // use session timeout
	    timeout = this->default_timeout * 1000;
	}
    }

    // Indicate that we are waiting for a specific response:
    // We add a null pointer to the map
    responses[packetID] = boost::shared_ptr<ResponsePDU>();

    // Start timeout timer
    boost::asio::deadline_timer timer(*(this->io_service));
    status_t timer_result = in_progress; // callback stores result here
    timer.expires_from_now( boost::posix_time::milliseconds(timeout) );
    timer.async_wait( boost::bind(callback_for_response,
				  boost::asio::placeholders::error,
				  &timer_result) );

    // process asio events until ResponsePDU arrives or timeout expires
    while(   this->responses[packetID].get() == 0 // no response was stored
	  && timer_result == in_progress)
    {
	this->io_service->run_one();
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
	throw(timeout_exception());
    }
}
