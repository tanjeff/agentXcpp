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

session::session(std::string filename)
    : endpoint(filename.c_str()),
      socket(io_service)
{
    cout << filename << endl;

    // Connect to endpoint
    socket.connect(endpoint);
    
    // Send OpenPDU
    OpenPDU openpdu;
    data_t buf = openpdu.serialize();
    socket.send(boost::asio::buffer(buf.c_str(), buf.size()));

    // Get response
    PDU* response = PDU::get_pdu(socket);

    // Get sessionID
    sessionID = response->get_sessionID();

    cout << "received sessionID " << sessionID << endl;
}

session::~session()
{
    cout << "session::~session(): sending ClosePDU" << endl;
    
    ClosePDU pdu(sessionID, ClosePDU::reasonShutdown);
    data_t buf = pdu.serialize();
    socket.send(boost::asio::buffer(buf.c_str(), buf.size()));
    socket.close();
}
