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

#include "IpAddress.h"

using namespace agentxcpp;

data_t IpAddress::serialize() const
{
    data_t serialized;

    // encode size (big endian) (size is always 4)
    serialized.push_back(0);
    serialized.push_back(0);
    serialized.push_back(0);
    serialized.push_back(4);

    // encode address
    serialized.push_back(address[0] >> 24 & 0xff);
    serialized.push_back(address[0] >> 16 & 0xff);
    serialized.push_back(address[0] >> 8 & 0xff);
    serialized.push_back(address[0] >> 0 & 0xff);
    serialized.push_back(address[1] >> 24 & 0xff);
    serialized.push_back(address[1] >> 16 & 0xff);
    serialized.push_back(address[1] >> 8 & 0xff);
    serialized.push_back(address[1] >> 0 & 0xff);
    serialized.push_back(address[2] >> 24 & 0xff);
    serialized.push_back(address[2] >> 16 & 0xff);
    serialized.push_back(address[2] >> 8 & 0xff);
    serialized.push_back(address[2] >> 0 & 0xff);
    serialized.push_back(address[3] >> 24 & 0xff);
    serialized.push_back(address[3] >> 16 & 0xff);
    serialized.push_back(address[3] >> 8 & 0xff);
    serialized.push_back(address[3] >> 0 & 0xff);

    return serialized;
}


IpAddress::IpAddress(data_t::const_iterator& pos,
		     const data_t::const_iterator& end,
		     bool big_endian)
{
    uint32_t size;
    
    // Are there at least 20 bytes in the buffer?
    if(end - pos < 20)
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
    if( size != 16 )
    {
	// Parse error; this seems not to be an IpAddress
	throw parse_error();
    }

    // Get address
    if( big_endian )
    {
	address[0] =  *pos++ << 24;
	address[0] |= *pos++ << 16;
	address[0] |= *pos++ << 8;
	address[0] |= *pos++ << 0;
	address[1] =  *pos++ << 24;
	address[1] |= *pos++ << 16;
	address[1] |= *pos++ << 8;
	address[1] |= *pos++ << 0;
	address[2] =  *pos++ << 24;
	address[2] |= *pos++ << 16;
	address[2] |= *pos++ << 8;
	address[2] |= *pos++ << 0;
	address[3] =  *pos++ << 24;
	address[3] |= *pos++ << 16;
	address[3] |= *pos++ << 8;
	address[3] |= *pos++ << 0;
    }
    else
    {
	address[0] =  *pos++ << 0;
	address[0] |= *pos++ << 8;
	address[0] |= *pos++ << 16;
	address[0] |= *pos++ << 24;
	address[1] =  *pos++ << 0;
	address[1] |= *pos++ << 8;
	address[1] |= *pos++ << 16;
	address[1] |= *pos++ << 24;
	address[2] =  *pos++ << 0;
	address[2] |= *pos++ << 8;
	address[2] |= *pos++ << 16;
	address[2] |= *pos++ << 24;
	address[3] =  *pos++ << 0;
	address[3] |= *pos++ << 8;
	address[3] |= *pos++ << 16;
	address[3] |= *pos++ << 24;
    }
}
