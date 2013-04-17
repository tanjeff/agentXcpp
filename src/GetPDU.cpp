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

#include "GetPDU.hpp"


using namespace agentxcpp;

GetPDU::GetPDU(binary::const_iterator& pos,
		const binary::const_iterator& end,
		bool big_endian)
    : PDUwithContext(pos, end, big_endian)
{
    // Get SearchRanges until the PDU is completely parsed
    while( pos < end )
    {
	// read starting OidValue
	sr.push_back(OidValue(pos, end, big_endian));

	// read and forget ending OidValue (but check its include field)
	OidValue ending(pos, end, big_endian);
	if(ending.get_include() == true)
	{
	    // Parse error according to RFC 2741, 5.2 "SearchRange":
	    // include field of ending OID must be 0
	    throw( parse_error() );
	}



	pos += 4;   // ignore empty "end" OidValue
    }
}
	    



binary GetPDU::serialize() const
{
    binary serialized;

    // Add OID's
    vector<OidValue>::const_iterator i;
    for(i = sr.begin(); i < sr.end(); i++)
    {
	serialized += i->serialize();
    }

    // Add header
    add_header(PDU::agentxGetPDU, serialized);

    // return serialized form of PDU
    return serialized;
}
