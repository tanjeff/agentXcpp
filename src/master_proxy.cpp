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
    connection(new connector(shared_ptr<boost::asio::io_service>(_io_service),
			     _filename.c_str(),
			     _default_timeout)),
    description(_description),
    default_timeout(_default_timeout),
    id(_id)
{
    // Try to connect
    try
    {
	// throws disconnected:
	connect();
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
    connection(new connector(shared_ptr<boost::asio::io_service>(io_service),
			     _filename.c_str(),
			     _default_timeout)),
    description(_description),
    default_timeout(_default_timeout),
    id(_id)
{
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

    // Try clean shutdown
    try
    {
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
    disconnect(ClosePDU::reasonShutdown);

    // Destroy connection
    delete connection;

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
    this->connection->send(pdu);

    // Wait for response
    boost::shared_ptr<ResponsePDU> response;
    try{
	response = this->connection->wait_for_response(pdu.get_packetID());
    }
    catch(timeout_error e)
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


