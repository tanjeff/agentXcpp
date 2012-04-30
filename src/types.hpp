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
#ifndef _TYPES_H_
#define _TYPES_H_

#include <string>
#include <istream>
#include <ostream>

typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef signed int int32_t;
typedef unsigned long long uint64_t;

/**
 * \brief A type with exactly 8 bits
 *
 * The char types in C++ (char, unsigned char, signed char) are defined to have 
 * <em>at least</em> 8 bits. Although these types probably have 8 bits on 
 * virtually every platform, I'm a perfectionist here and define my own type, 
 * so that it can be redefined in one central place.
 */
typedef unsigned char byte_t; // for machines where unsigned char has 8bits

/**
 * \brief A type representing a contigous byte stream
 */
class data_t : public std::basic_string<byte_t> { };

inline std::ostream& operator<<(std::ostream& out, const data_t& stream)
{
    out << "+----------+----------+----------+----------+" << std::endl;
    out << "| ";//begin line
    for(size_t i = 0; i < stream.size(); i++)
    {
	out.width(8);// 8 chars per field
	out << (int)stream[i] << " | ";
	if( (i+1)%4 == 0 )
	{
	    out << std::endl;   // end line
	    out << "+----------+----------+----------+----------+";
	    if( i != stream.size() - 1 )
	    {
		// We have further data; begin a new line
		out << std::endl << "| ";
	    }
	}
    }
    out << std::endl;

    return out;
}

#endif
