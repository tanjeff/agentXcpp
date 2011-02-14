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

#include "RegisterPDU.h"
#include "helper.h"

using namespace agentx;

RegisterPDU::RegisterPDU(
	data_t::const_iterator& pos,
	bool big_endian) throw(parse_error)
    :PDU(pos, big_endian)   // parse header
{
    timeout = read32(pos, big_endian);
    priority = read32(pos, big_endian);
    range_subid = read32(pos, big_endian);
    pos++;  // skip reserved field

    subtree = oid(pos, big_endian);

    // read r.upper_bound only if r.range_subid is not 0
    if( range_subid )
    {
	upper_bound = oid(pos, big_endian);
    }
}


data_t RegisterPDU::serialize()
{
    data_t serialized;

    serialized.push_back(timeout);
    serialized.push_back(priority);
    serialized.push_back(range_subid);
    serialized.push_back(0);	// reserved

    serialized += subtree.serialize();

    if( range_subid )
    {
	serialized += upper_bound.serialize();
    }

    // Add Header
    add_header(3, serialized);

    return serialized;

}
