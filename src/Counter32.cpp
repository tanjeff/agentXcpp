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

#include "Counter32.hpp"
#include "util.hpp"

using namespace agentxcpp;

binary Counter32::serialize() const
{
    binary serialized;

    // encode value (big endian)
    write32(serialized, value);

    return serialized;
}


Counter32::Counter32(binary::const_iterator& pos,
		     const binary::const_iterator& end,
		     bool big_endian)
{
    // Are there at least 4 bytes in the buffer?
    if(end - pos < 4)
    {
	throw(parse_error());
    }

    // Get value
    value = read32(pos, big_endian);
}
