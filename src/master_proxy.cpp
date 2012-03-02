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






master_proxy::master_proxy(boost::asio::io_service* _io_service,
			   std::string _description,
			   byte_t _default_timeout,
			   oid _id,
			   std::string _filename) :
    io_service(_io_service),
    io_service_by_user(true),
    socket_file(_filename.c_str()),
    description(_description),
    default_timeout(_default_timeout),
    id(_id)
{
    // Initialize connector (never use timeout=0)
    byte_t timeout;
    timeout = (this->default_timeout == 0) ? 1 : this->default_timeout;
    connection = new connector(shared_ptr<boost::asio::io_service>(io_service),
			       _filename.c_str(),
			       timeout);
		      
    // Register this object as %PDU handler
    this->connection->register_handler( this );

    // Try to connect
    try
    {
	// throws disconnected:
	this->connect();
    }
    catch(disconnected)
    {
	// Ignore, stay disconnected
    }
}



master_proxy::master_proxy(std::string _description,
			   byte_t _default_timeout,
			   oid _id,
			   std::string _filename) :
    io_service(new boost::asio::io_service()),
    io_service_by_user(false),
    socket_file(_filename.c_str()),
    description(_description),
    default_timeout(_default_timeout),
    id(_id)
{
    // Initialize connector (never use timeout=0)
    byte_t timeout;
    timeout = (this->default_timeout == 0) ? 1 : this->default_timeout;
    connection = new connector(shared_ptr<boost::asio::io_service>(io_service),
			       _filename.c_str(),
			       timeout);
		      
    // Register this object as %PDU handler
    this->connection->register_handler( this );

    // Try to connect
    try
    {
	// throws disconnected:
	this->connect();
    }
    catch(disconnected)
    {
	// Ignore, stay disconnected
    }
    catch(...)
    {
	// Ignore, stay disconnected
    }
    
    
}


void master_proxy::connect()
{
    if( this->connection->is_connected() )
    {
	// we are already connected -> nothing to do
	return;
    }

    // Clear registrations
    registrations.clear();

    // Connect to endpoint
    try
    {
	this->connection->connect();
    }
    catch(boost::system::system_error)
    {
    	throw;
    }
    // The response we expect from the master
    boost::shared_ptr<ResponsePDU> response;

    try
    {
	// Send OpenPDU
	OpenPDU openpdu;
	openpdu.set_timeout(default_timeout);
	openpdu.set_id(id);
	// throws disconnected and timeout_error:
	this->connection->send(openpdu);

	// Wait for response
	// throws disconnected and timeout_error:
	response = this->connection->wait_for_response(openpdu.get_packetID());
    }
    catch(disconnected)
    {
	throw; // forward
    }
    catch(timeout_error)
    {
	throw disconnected();
    }

    // Check for errors
    if(response->get_error() != ResponsePDU::noAgentXError)
    {
	// Some error occured, disconnect
	this->connection->disconnect();
	throw disconnected();
    }

    // All went fine, we are connected now
    this->sessionID = response->get_sessionID();
}




void master_proxy::disconnect(ClosePDU::reason_t reason)
{
    if( ! this->connection->is_connected() )
    {
	// we are already disconnected -> nothing to do
	return;
    }

    // According to RFC 2741, 7.1.8. "Processing the agentx-Close-PDU", the 
    // master agent unregisters all MIB regions, frees all index values and all 
    // sysORID are removed. Thus no need to clean up before ClosePDU is sent.
    
    // The response we expect from the master
    boost::shared_ptr<ResponsePDU> response;

    // Try clean shutdown (ignore errors)
    try
    {
	// Unregister stuff if any
	std::list< boost::shared_ptr<RegisterPDU> >::const_iterator r;
	r = this->registrations.begin();
	while (r != this->registrations.end())
	{
	    this->undo_registration(create_unregister_pdu(*r));
	    r++;
	}

	// Send ClosePDU
	ClosePDU closepdu(this->sessionID, reason);
	// throws disconnected and timeout_error:
	this->connection->send(closepdu);
	
	// Wait for response
	// throws disconnected and timeout_error:
	response = this->connection->wait_for_response(closepdu.get_packetID());
    
	// Check for errors
	if(response->get_error() != ResponsePDU::noAgentXError)
	{
	    // Some error occured. However, We will soon disconnect anyway.
	    // -> ignore
	    // TODO: Insert log message here.
	}
    }
    catch(...)
    {
	// We will soon disconnect anyway.
	// -> ignore all errors
    }

    // Finally: disconnect
    this->connection->disconnect();
}

master_proxy::~master_proxy()
{
    // Disconnect from master agent
    this->disconnect(ClosePDU::reasonShutdown);
    
    // Destroy connection
    // Unregistering this object as %PDU handler is unneeded.
    delete this->connection;

    // Destroy io_service object if needed
    if( ! this->io_service_by_user )
    {
	delete this->io_service;
    }
}


void master_proxy::do_registration(boost::shared_ptr<RegisterPDU> pdu)
{
    // Are we connected?
    if( ! is_connected())
    {
	throw(disconnected());
    }

    // Send RegisterPDU
    // (forward exceptions timeout_error and disconnected)
    this->connection->send(*pdu);

    // Wait for response
    // (forward exceptions timeout_error and disconnected)
    boost::shared_ptr<ResponsePDU> response;
    response = this->connection->wait_for_response(pdu->get_packetID());

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
	    // Hey, it worked!
	    break;

	// Register-specific errors:

	case ResponsePDU::duplicateRegistration:
	    throw(duplicate_registration());

	case ResponsePDU::requestDenied:
	    throw(master_is_unwilling());

	default:
	    // This is a case of can-not-happen. Probably the master is buggy.  
	    // The agentxcpp library is bug-free of course ;-)
	    // We throw a parse error meanwhile, because we didn't expect the 
	    // response to look like that...
	    throw(parse_error());
    }

    // Finish
    return;
}





void master_proxy::register_subtree(oid subtree,
		      byte_t priority,
		      byte_t timeout)
{
    // Build PDU
    boost::shared_ptr<RegisterPDU> pdu(new RegisterPDU);
    pdu->set_subtree(subtree);
    pdu->set_priority(priority);
    pdu->set_timeout(timeout);
    
    // Sent PDU
    try
    {
	this->do_registration(pdu);
    }
    catch( internal_error )
    {
	// Huh, it seems that we sent a malformed PDU to the master. We convert 
	// this to parse_error.
	throw(parse_error());
    }
    catch(...)
    {
	// All other exceptions are forwarded unmodified:
	throw;
    }

    // Success: store registration
    this->registrations.push_back(pdu);

}



void master_proxy::unregister_subtree(oid subtree,
				      byte_t priority)
{
    // The UnregisterPDU
    boost::shared_ptr<UnregisterPDU> pdu;

    // Remove the registration from registrations list
    std::list< boost::shared_ptr<RegisterPDU> >::iterator r;
    r = this->registrations.begin();
    while (r != this->registrations.end())
    {
	if(   (*r)->get_priority() == priority
	   && (*r)->get_subtree() == subtree
	   && (*r)->get_range_subid() == 0
	   && (*r)->get_upper_bound() == 0 )
	{
	    // registration found

	    // create UnregisterPDU
	    pdu = create_unregister_pdu(*r);

	    // remove registration from list, forward to next one
	    r = registrations.erase(r);
	}
	else
	{
	    // Ignore registration, inspect next one
	    r++;
	}
    }

    // Sent PDU
    try
    {
	this->undo_registration(pdu);
    }
    catch( internal_error )
    {
	// Huh, it seems that we sent a malformed PDU to the master. We convert 
	// this to parse_error.
	throw(parse_error());
    }
    catch(...)
    {
	// All other exceptions are forwarded unmodified:
	throw;
    }
}



void master_proxy::undo_registration(boost::shared_ptr<UnregisterPDU> pdu)
{
    // Are we connected?
    if( ! is_connected())
    {
	throw(disconnected());
    }

    // Send UnregisterPDU
    // (forward exceptions timeout_error and disconnected)
    this->connection->send(*pdu);

    // Wait for response
    // (forward exceptions timeout_error and disconnected)
    boost::shared_ptr<ResponsePDU> response;
    response = this->connection->wait_for_response(pdu->get_packetID());

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
	    // Hey, it worked!
	    break;

	// Register-specific errors:

	case ResponsePDU::unknownRegistration:
	    throw(unknown_registration());

	default:
	    // This is a case of can-not-happen. Probably the master is buggy.  
	    // The agentxcpp library is bug-free of course ;-)
	    // We throw a parse error meanwhile, because we didn't expect the 
	    // response to look like that...
	    throw(parse_error());
    }

    // Finish
    return;
}



boost::shared_ptr<UnregisterPDU> master_proxy::create_unregister_pdu(
				    boost::shared_ptr<RegisterPDU> pdu)
{
    boost::shared_ptr<UnregisterPDU> new_pdu(new UnregisterPDU());
    new_pdu->set_subtree( pdu->get_subtree() );
    new_pdu->set_range_subid( pdu->get_range_subid() );
    new_pdu->set_upper_bound( pdu->get_upper_bound() );
    new_pdu->set_priority( pdu->get_priority() );

    return new_pdu;
}

