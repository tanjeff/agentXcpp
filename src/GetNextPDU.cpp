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

#include "GetNextPDU.hpp"


using namespace agentxcpp;

GetNextPDU::GetNextPDU(data_t::const_iterator& pos,
		       const data_t::const_iterator& end,
		       bool big_endian)
    : PDUwithContext(pos, end, big_endian)
{
    // Get SearchRanges until the PDU is completely parsed
    while( pos < end )
    {
	pair<oid,oid> p;

	p.first = oid(pos, end, big_endian);  // starting oid
	p.second = oid(pos, end, big_endian); // ending oid

	if(p.second.get_include() == true)
	{
	    // Parse error according to RFC 2741, 5.2 "SearchRange":
	    // include field of ending OID must be 0
	    throw( parse_error() );
	}

	sr.push_back(p);
    }
}
	    



data_t GetNextPDU::serialize() const
{
    data_t serialized;

    // Add OID's
    vector< pair<oid,oid> >::const_iterator i;
    for(i = sr.begin(); i < sr.end(); i++)
    {
	serialized += i->first.serialize();
	serialized += i->second.serialize();
    }

    // Add header
    add_header(PDU::agentxGetNextPDU, serialized);

    // return serialized form of PDU
    return serialized;
}
