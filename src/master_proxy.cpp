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
	try
	{
	    socket.close();
	}
	catch(...)
	{
	}
	return;
    }
    
    // Send OpenPDU
    OpenPDU openpdu;
    openpdu.set_timeout(default_timeout);
    openpdu.set_id(id);
    data_t buf = openpdu.serialize();
    socket.send(asio::buffer(buf.data(), buf.size()));

    // Wait for response
    ResponsePDU* response;
    try
    {
	// TODO: memory leak!
	response = dynamic_cast<ResponsePDU*>(PDU::get_pdu(socket));
    }
    catch(...)
    {
	// ignore errors from PDU::get_pdu()
    }

    // Check for errors
    if(response == 0)
    {
	// Expected ResponsePDU, but received other PDU
	try
	{
	    socket.close();
	}
	catch(...)
	{
	}
	return;
    }
    if(response->get_error() != ResponsePDU::noAgentXError)
    {
	// Some error occured, disconnect
	try
	{
	    socket.close();
	}
	catch(...)
	{
	}
	return;
    }

    // If it worked: get sessionID
    this->sessionID = response->get_sessionID();

    // Finally: start receiving (the callback will call async_read() to 
    // continue receiving)
    async_read(this->socket,
	       boost::asio::buffer(this->header_buf, 20),
	       boost::bind(&master_proxy::receive,
			   this,
			   boost::asio::placeholders::error));
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
	try
	{
	    socket.close();
	}
	catch(...)
	{
	}
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
	try { this->socket.close(); } catch(...) { }

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
	try { this->socket.close(); } catch(...) { }
    }

    // Read the payload (TODO: can we avoid the new() operator?)
    byte_t* payload = new byte_t[payload_length];
    boost::asio::read(this->socket,
		      boost::asio::buffer(payload, payload_length));
    buf.append(payload, payload_length);
    delete[] payload;

    // Parse PDU
    auto_ptr<PDU> pdu;
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
	try { this->socket.close(); } catch(...) { }
    }

    // Dispatch!
    if( dispatch_pdu )
    {
	// We use dynamic_cast to find out which type of PDU we received.

	// Get the real pointer. We take the ownership!
	PDU* pdu_ptr = pdu.release();

	
	if(ResponsePDU* response_ptr = dynamic_cast<ResponsePDU*>(pdu_ptr))
	{
	    // was a ResponsePDU: store to the responses map
	    boost::shared_ptr<ResponsePDU> response(response_ptr);
	    this->responses[response_ptr->get_packetID()] = response;
	    // TODO: What if a ResponsePDU is already stored with that 
	    // packetID?
	}
	else if(GetPDU* get_ptr = dynamic_cast<GetPDU*>(pdu_ptr))
	{
	    // was a GtPDU: for now, we simply put something on the console.
	    boost::shared_ptr<GetPDU> get(get_ptr);
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




// Helper type for master_proxy::wait_for_response(). See there for an 
// explanation.
enum status_t {
    in_progress = 0,
    success     = 1,
    fail        = 2
};


// Helper function for master_proxy::wait_for_response(). See there for an 
// explanation.
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
/*
 * Timeout handling:
 * 
 * The timeout handling is realized using a boost::asio::deadline_timer 
 * object, which is set up to call the callback() function when it expires (or 
 * when it fails). The variable 'timer_result' is intially set to 
 * 'in_progress'.  The callback will set it to 'success' or 'fail' when the 
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
    // Handle default timeout (omitting timeout parameter)
    if( timeout == 0 )
    {
	if(this->default_timeout == 0)
	{
    cout << __FILE__ << ":" << __LINE__ << endl;
	    // No timeout given: fall back to 1 second
	    timeout = 1000;
	}
	else
	{
    cout << __FILE__ << ":" << __LINE__ << endl;
	    // No timeout given to function, but we have a session timeout: 
	    // use session timeout
	    timeout = this->default_timeout * 1000;
	}
    }

    // Start timeout timer
    boost::asio::deadline_timer timer(*(this->io_service));
    status_t timer_result = in_progress; // callback stores result here
    timer.expires_from_now( boost::posix_time::milliseconds(timeout) );
    timer.async_wait( boost::bind(callback,
				  boost::asio::placeholders::error,
				  &timer_result) );

    // process asio events until ResponsePDU arrives or timeout expires
    while(   this->responses.find(packetID) == this->responses.end()
	  && timer_result == in_progress)
    {
	this->io_service->run_one();
    }
    
    // Check the result
    map< uint32_t, boost::shared_ptr<ResponsePDU> >::iterator response;
    response = this->responses.find(packetID);
    if(response != this->responses.end())
    {
	// ResponsePDU arrived:
	// 1. Cancel timer
	// 2. Erase response from map
	// 3. Return response
	timer.cancel();
	shared_ptr<ResponsePDU> retval = (*response).second;
	this->responses.erase(response);
	return retval;
    }
    else
    {
	// Timer expired or failed before ResponsePDU arrived
	throw(timeout_exception());
    }
}
