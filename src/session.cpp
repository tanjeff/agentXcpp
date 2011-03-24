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
#include "session.h"
#include "OpenPDU.h"
#include "ClosePDU.h"
#include "helper.h"
#include "types.h"

#include <iostream>
using namespace std;

using namespace agentx;
using namespace boost;


    
session::session(std::string descr, byte_t timeout, oid _id, std::string filename) :
    socket(io_service),
    endpoint(filename.c_str()),
    description(descr),
    default_timeout(timeout),
    id(_id)
{
    connect();
}


void session::connect()
{
    if(connected)
    {
	// we are already connected -> nothing to do
	return;
    }

    // Connect to endpoint
    socket.connect(endpoint);
    
    // Send OpenPDU
    OpenPDU openpdu = OpenPDU(Octet_String(), default_timeout, id);
    data_t buf = openpdu.serialize();
    socket.send(asio::buffer(buf.data(), buf.size()));

    // Wait for response
    PDU* response = PDU::get_pdu(socket);

    // Get sessionID
    sessionID = response->get_sessionID();

    // We are now connected
    connected = true;

    cout << "received sessionID " << sessionID << endl;
}

void session::disconnect(ClosePDU::reason_t reason)
{
    if( !connected )
    {
	// we are already disconnected -> nothing to do
	return;
    }

    // Send ClosePDU
    ClosePDU pdu(sessionID, reason);
    data_t buf = pdu.serialize();
    socket.send(asio::buffer(buf.c_str(), buf.size()));

    // Wait for response
    PDU* response = PDU::get_pdu(socket);

    // Close socket
    socket.close();

    // We are now disconnected
    connected = false;
}

session::~session()
{
    cout << "session::~session(): sending ClosePDU" << endl;
    
    disconnect(ClosePDU::reasonShutdown);
}
