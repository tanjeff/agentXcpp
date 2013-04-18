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

#include <boost/cstdint.hpp>

#include "Counter64Value.hpp"
#include "util.hpp"

using namespace agentxcpp;
using boost::uint64_t;

binary Counter64Value::serialize() const
{
    binary serialized;

    // encode value (big endian)
    write64(serialized, value);

    return serialized;
}


Counter64Value::Counter64Value(binary::const_iterator& pos,
		    const binary::const_iterator& end,
		    bool big_endian)
{
    // Are there at least 8 bytes in the buffer?
    if(end - pos < 8)
    {
	throw(parse_error());
    }


    // Get value
    value = read64(pos, big_endian);
}
