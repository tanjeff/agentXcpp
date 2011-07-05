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
#include "UnregisterPDU.hpp"
#include "helper.hpp"

using namespace agentxcpp;

UnregisterPDU::UnregisterPDU(data_t::const_iterator& pos,
			     const data_t::const_iterator& end,
			     bool big_endian)
    :PDU(pos, end, big_endian)   // parse header
{
    if(end - pos < 4)
    {
	throw(parse_error());
    }
    pos++;  // skip reserved field
    priority = *pos++;
    range_subid = *pos++;
    pos++;  // skip reserved field

    subtree = oid(pos, end, big_endian);

    // read r.upper_bound only if r.range_subid is not 0
    if( range_subid )
    {
	upper_bound = oid(pos, end, big_endian);
    }
}


data_t UnregisterPDU::serialize()
{
    data_t serialized;

    serialized.push_back(0);	// reserved
    serialized.push_back(this->priority);
    serialized.push_back(this->range_subid);
    serialized.push_back(0);	// reserved

    serialized += subtree.serialize();

    if( range_subid )
    {
	serialized += upper_bound.serialize();
    }

    // Add Header
    add_header(PDU::agentxUnregisterPDU, serialized);

    return serialized;

}


UnregisterPDU::UnregisterPDU()
    : priority(127), range_subid(0), subtree(), upper_bound()
{
}
