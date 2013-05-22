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

#include "IpAddressValue.hpp"
#include "util.hpp"

using namespace agentxcpp;

binary IpAddressValue::serialize() const
{
    binary serialized;

    // encode size (big endian) (size is always 4)
    serialized.push_back(0);
    serialized.push_back(0);
    serialized.push_back(0);
    serialized.push_back(4);

    // encode address
    serialized.push_back(address[0]);
    serialized.push_back(address[1]);
    serialized.push_back(address[2]);
    serialized.push_back(address[3]);

    return serialized;
}


IpAddressValue::IpAddressValue(binary::const_iterator& pos,
		     const binary::const_iterator& end,
		     bool big_endian)
{
    quint32 size;
    
    // Are there at least 8 bytes in the buffer?
    if(end - pos < 8)
    {
	throw(parse_error());
    }

    // Get size
    size = read32(pos, big_endian);
    if( size != 4 )
    {
	// Parse error; this seems not to be an IpAddress
	throw parse_error();
    }

    // Get address
    // always most significant first according to 
    // RFC2741, 5.4 "Value Representation"
    address[0] =  *pos++;
    address[1] =  *pos++;
    address[2] =  *pos++;
    address[3] =  *pos++;
}
