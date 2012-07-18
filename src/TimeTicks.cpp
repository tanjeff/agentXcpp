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

#include "TimeTicks.hpp"
#include "util.hpp"

using namespace agentxcpp;

data_t TimeTicks::serialize() const
{
    data_t serialized;

    // encode value (big endian)
    write32(serialized, value);

    return serialized;
}


TimeTicks::TimeTicks(data_t::const_iterator& pos,
		     const data_t::const_iterator& end,
		     bool big_endian)
{
    // We need 4 bytes
    if(end - pos < 4)
    {
	throw(parse_error());
    }

    // Get value
    value = read32(pos, big_endian);
}
