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

#include <QtGlobal>

#include "binary.hpp"


namespace agentxcpp
{

    inline quint64 read64(binary::const_iterator& pos, bool big_endian)
    {
        quint64 value;
        if( big_endian )
        {
            value =  static_cast<quint64>(*pos++) << 56;
            value |= static_cast<quint64>(*pos++) << 48;
            value |= static_cast<quint64>(*pos++) << 40;
            value |= static_cast<quint64>(*pos++) << 32;
            value |= static_cast<quint64>(*pos++) << 24;
            value |= static_cast<quint64>(*pos++) << 16;
            value |= static_cast<quint64>(*pos++) << 8;
            value |= static_cast<quint64>(*pos++) << 0;
        }
        else
        {
            value =  static_cast<quint64>(*pos++) << 0;
            value |= static_cast<quint64>(*pos++) << 8;
            value |= static_cast<quint64>(*pos++) << 16;
            value |= static_cast<quint64>(*pos++) << 24;
            value |= static_cast<quint64>(*pos++) << 32;
            value |= static_cast<quint64>(*pos++) << 40;
            value |= static_cast<quint64>(*pos++) << 48;
            value |= static_cast<quint64>(*pos++) << 56;
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
    inline void write64(binary& serialized, quint64 value)
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


    inline quint32 read32(binary::const_iterator& pos, bool big_endian)
    {
        quint32 value;
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
    inline void write32(binary& serialized, quint32 value)
    {
        // always big endian
        serialized.push_back(value >> 24 & 0xff);
        serialized.push_back(value >> 16 & 0xff);
        serialized.push_back(value >> 8 & 0xff);
        serialized.push_back(value >> 0 & 0xff);
    }

    inline quint16 read16(binary::const_iterator& pos, bool big_endian)
    {
        quint16 value = 0;
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
    inline void write16(binary& serialized, quint16 value)
    {
        // always big endian
        serialized.push_back(value >> 8 & 0xff);
        serialized.push_back(value >> 0 & 0xff);
    }
}

#endif
