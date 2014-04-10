/*
 * Copyright 2011-2012 Tanjeff-Nicolai Moos <tanjeff@cccmz.de>
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

#include "OpenPDU.hpp"
#include "OidVariable.hpp"

using namespace agentxcpp;

OpenPDU::OpenPDU(binary::const_iterator& pos,
		 const binary::const_iterator& end,
		 bool big_endian)
    : PDU(pos, end, big_endian)
{
    if(end - pos < 4)
    {
	throw(parse_error());
    }

    // header is parsed by base class constructor
    // so we continue with timeout field:
    timeout = *pos++;
    pos += 3;	// skip reserved fields

    id = OidVariable(pos, end, big_endian).value();

    descr = OctetStringVariable(pos, end, big_endian);
}
	    

OpenPDU::OpenPDU()
    : timeout(0), id(), descr("")
{
}


binary OpenPDU::serialize() const
{
    binary serialized;

    // timeout and reserved fields
    serialized.push_back(timeout);
    serialized.push_back(0);
    serialized.push_back(0);
    serialized.push_back(0);

    // id
    serialized += OidVariable(id).serialize();

    // descr
    serialized += descr.serialize();

    // Add header (type for OpenPDU is 1)
    add_header(PDU::agentxOpenPDU, serialized);

    // return serialized form of PDU
    return serialized;
}
