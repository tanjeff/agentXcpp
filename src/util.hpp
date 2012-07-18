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

#include <boost/cstdint.hpp>

#include "binary.hpp"

using boost::uint16_t;
using boost::uint32_t;

namespace agentxcpp
{

    inline uint64_t read64(binary::const_iterator& pos, bool big_endian)
    {
        uint64_t value;
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
        return value;
    }


    /**
     * \brief Write a 64-bit value into a string
     *
     * \param serialized The string to which the value is appended.
     *
     * \param value The value which is appended to the string.
     */
    inline void write64(binary& serialized, uint64_t value)
    {
        // always big endian
        serialized.push_back(value >> 56 & 0xff);
        serialized.push_back(value >> 48 & 0xff);
        serialized.push_back(value >> 40 & 0xff);
        serialized.push_back(value >> 32 & 0xff);
        serialized.push_back(value >> 24 & 0xff);
        serialized.push_back(value >> 16 & 0xff);
        serialized.push_back(value >> 8 & 0xff);
        serialized.push_back(value >> 0 & 0xff);
    }


    inline uint32_t read32(binary::const_iterator& pos, bool big_endian)
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
    inline void write32(binary& serialized, uint32_t value)
    {
        // always big endian
        serialized.push_back(value >> 24 & 0xff);
        serialized.push_back(value >> 16 & 0xff);
        serialized.push_back(value >> 8 & 0xff);
        serialized.push_back(value >> 0 & 0xff);
    }

    inline uint16_t read16(binary::const_iterator& pos, bool big_endian)
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
    inline void write16(binary& serialized, uint16_t value)
    {
        // always big endian
        serialized.push_back(value >> 8 & 0xff);
        serialized.push_back(value >> 0 & 0xff);
    }
}

#endif
