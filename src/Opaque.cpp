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

#include "Opaque.hpp"

using namespace agentxcpp;

data_t Opaque::serialize() const
{
    data_t serialized;

    // encode size (big endian)
    int size = value.size();
    serialized.push_back(size >> 24 & 0xff);
    serialized.push_back(size >> 16 & 0xff);
    serialized.push_back(size >> 8 & 0xff);
    serialized.push_back(size >> 0 & 0xff);

    // encode value
    serialized += value;

    // Padding bytes
    int padsize = 4 - (size % 4);
    while( padsize-- )
    {
	serialized.push_back(0);
    }

    return serialized;
}


Opaque::Opaque(data_t::const_iterator& pos,
	       const data_t::const_iterator& end,
	       bool big_endian)
{
    int size;
    
    // We need 4 bytes for the size
    if(end - pos < 4)
    {
	throw(parse_error());
    }

    // Get size
    if( big_endian )
    {
	size =  *pos++ << 24;
	size |= *pos++ << 16;
	size |= *pos++ << 8;
	size |= *pos++ << 0;
    }
    else
    {
	size =  *pos++ << 0;
	size |= *pos++ << 8;
	size |= *pos++ << 16;
	size |= *pos++ << 24;
    }
    
    // Octet String emtpy?
    if( size == 0 )
    {
	// nothing left to parse
	return;
    }

    // We want to read (size) more bytes
    if((end - pos) < size)
    {
	throw(parse_error());
    }
    
    // Get value
    value.assign(pos, pos+size);
    pos += size;
    
    // Eat padding bytes
    int padsize = 4 - (size % 4);
    if( padsize == 4 ) padsize = 0;
    if(end - pos < padsize)
    {
	throw(parse_error());
    }
    while( padsize-- )
    {
	pos++;
    }
}
