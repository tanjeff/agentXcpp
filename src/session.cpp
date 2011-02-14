/*
 * Copyright 2011 Tanjeff-Nicolai Moos <tanjeff@cccmz.de>
 *
 * This file is part of agentXcpp.
 *
 * AgentXcpp is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * AgentXcpp is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with AgentXcpp.  If not, see <http://www.gnu.org/licenses/>.
 *
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
: socket(io_service),
    endpoint(filename.c_str())
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
