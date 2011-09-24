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
#include "master_proxy.hpp"
#include "OpenPDU.hpp"
#include "ClosePDU.hpp"
#include "ResponsePDU.hpp"
#include "helper.hpp"
#include "types.hpp"

#include <iostream>
using namespace std;

using namespace agentxcpp;
using namespace boost;


    
master_proxy::master_proxy(std::string descr,
			   byte_t timeout,
			   oid _id,
			   std::string filename) :
    io_service(),
    socket(io_service),
    endpoint(filename.c_str()),
    description(descr),
    default_timeout(timeout),
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
    disconnect(ClosePDU::reasonShutdown);
}
