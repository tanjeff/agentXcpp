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

#ifndef _HELPER_H_
#define _HELPER_H_

#include "types.hpp"

inline uint32_t read32(data_t::const_iterator& pos, bool big_endian)
{
    uint32_t value;
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

    return value;
}

/**
 * \brief Write a 32-bit value into a string
 *
 * \param serialized The string to which the value is appended.
 * 
 * \param value The value which is appended to the string.
 */
inline void write32(data_t& serialized, uint32_t value)
{
    // always big endian
    serialized.push_back(value >> 24 & 0xff);
    serialized.push_back(value >> 16 & 0xff);
    serialized.push_back(value >> 8 & 0xff);
    serialized.push_back(value >> 0 & 0xff);
}

inline uint16_t read16(data_t::const_iterator& pos, bool big_endian)
{
    uint16_t value = 0;
    if( big_endian )
    {
	value |= *pos++ << 8;
	value |= *pos++ << 0;
    }
    else
    {
	value =  *pos++ << 0;
	value |= *pos++ << 8;
    }

    return value;
}


/**
 * \brief Write a 16-bit value into a string
 *
 * \param serialized The string to which the value is appended.
 * 
 * \param value The value which is appended to the string.
 */
inline void write16(data_t& serialized, uint16_t value)
{
    // always big endian
    serialized.push_back(value >> 8 & 0xff);
    serialized.push_back(value >> 0 & 0xff);
}

#endif
