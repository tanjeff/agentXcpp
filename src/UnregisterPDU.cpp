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
#include "UnregisterPDU.hpp"
#include "util.hpp"

using namespace agentxcpp;

UnregisterPDU::UnregisterPDU(binary::const_iterator& pos,
			     const binary::const_iterator& end,
			     bool big_endian)
    :PDUwithContext(pos, end, big_endian)   // parse header
{
    if(end - pos < 4)
    {
	throw(parse_error());
    }
    pos++;  // skip reserved field
    priority = *pos++;
    range_subid = *pos++;
    pos++;  // skip reserved field

    subtree = OidValue(pos, end, big_endian);

    // read r.upper_bound only if r.range_subid is not 0
    if( range_subid )
    {
	if(end - pos < 4)
	{
	    throw(parse_error());
	}
	upper_bound = read32(pos, big_endian);
    }
}


binary UnregisterPDU::serialize() const
{
    binary serialized;

    serialized.push_back(0);	// reserved
    serialized.push_back(this->priority);
    serialized.push_back(this->range_subid);
    serialized.push_back(0);	// reserved

    serialized += subtree.serialize();

    if( range_subid )
    {
	write32(serialized, upper_bound);
    }

    // Add Header
    add_header(PDU::agentxUnregisterPDU, serialized);

    return serialized;

}


UnregisterPDU::UnregisterPDU()
    : priority(127), range_subid(0), subtree(), upper_bound(0)
{
}
