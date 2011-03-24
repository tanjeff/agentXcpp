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
#include "TimeTicks.h"

using namespace agentxcpp;

data_t TimeTicks::serialize()
{
    data_t serialized;

    // encode value (big endian)
    serialized.push_back(value >> 24 & 0xff);
    serialized.push_back(value >> 16 & 0xff);
    serialized.push_back(value >> 8 & 0xff);
    serialized.push_back(value >> 0 & 0xff);

    return serialized;
}


TimeTicks::TimeTicks(data_t::const_iterator& pos, bool big_endian)
{
    uint32_t size;

    // Get value
    if( big_endian )
    {
	value =  *pos++ << 24;
	value |= *pos++ << 16;
	value |= *pos++ << 8;
	value |= *pos++ << 0;
    }
    else
    {
	value =  *pos++ << 0;
	value |= *pos++ << 8;
	value |= *pos++ << 16;
	value |= *pos++ << 24;
    }
}
