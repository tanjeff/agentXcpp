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

#include "Counter64.h"

using namespace agentxcpp;

data_t Counter64::serialize()
{
    data_t serialized;

    // encode value (big endian)
    serialized.push_back(value >> 56 & 0xff);
    serialized.push_back(value >> 48 & 0xff);
    serialized.push_back(value >> 40 & 0xff);
    serialized.push_back(value >> 32 & 0xff);
    serialized.push_back(value >> 24 & 0xff);
    serialized.push_back(value >> 16 & 0xff);
    serialized.push_back(value >> 8 & 0xff);
    serialized.push_back(value >> 0 & 0xff);

    return serialized;
}


Counter64::Counter64(data_t::const_iterator& pos, bool big_endian)
{
    uint32_t size;

    // Get value
    if( big_endian )
    {
	value =  static_cast<uint64_t>(*pos++) << 56;
	value |= static_cast<uint64_t>(*pos++) << 48;
	value |= static_cast<uint64_t>(*pos++) << 40;
	value |= static_cast<uint64_t>(*pos++) << 32;
	value |= static_cast<uint64_t>(*pos++) << 24;
	value |= static_cast<uint64_t>(*pos++) << 16;
	value |= static_cast<uint64_t>(*pos++) << 8;
	value |= static_cast<uint64_t>(*pos++) << 0;
    }
    else
    {
	value =  static_cast<uint64_t>(*pos++) << 0;
	value |= static_cast<uint64_t>(*pos++) << 8;
	value |= static_cast<uint64_t>(*pos++) << 16;
	value |= static_cast<uint64_t>(*pos++) << 24;
	value |= static_cast<uint64_t>(*pos++) << 32;
	value |= static_cast<uint64_t>(*pos++) << 40;
	value |= static_cast<uint64_t>(*pos++) << 48;
	value |= static_cast<uint64_t>(*pos++) << 56;
    }
}
