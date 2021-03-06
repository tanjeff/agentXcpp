/*
 * Copyright 2011-2016 Tanjeff-Nicolai Moos <tanjeff@cccmz.de>
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

#include "ClosePDU.hpp"

using namespace agentxcpp;
	    

ClosePDU::ClosePDU()
    : reason(reasonOther)
{
}

ClosePDU::ClosePDU(quint32 _sessionID,
		   reason_t _reason)
{
    // Set sessionID
    this->set_sessionID(_sessionID);

    // Check parameter
    if( _reason < 1 || _reason > 6 )
    {
	throw(inval_param());
    }

    // store reason
    reason = _reason;
}


ClosePDU::ClosePDU(binary::const_iterator& pos,
		   const binary::const_iterator& end,
		   bool big_endian)
    : PDU(pos, end, big_endian)
{
    // header is parsed by base class constructor
    // so we continue with reason field:
    if(end - pos < 4)
    {
	throw(parse_error());
    }
    reason = static_cast<reason_t>(*pos++);
    pos += 3;	// skip reserved fields

    // check whether reason is valid
    if( reason < 1 || reason > 6)
    {
	// invalid
	throw(parse_error());
    }
}


binary ClosePDU::serialize() const
{
    binary serialized;

    // Encode reason and reserved fields
    serialized.push_back(reason);
    serialized.push_back(0);
    serialized.push_back(0);
    serialized.push_back(0);

    // Add Header
    add_header(PDU::agentxClosePDU, serialized);

    return serialized;
}
