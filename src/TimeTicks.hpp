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
#ifndef _TIMETICKS_H_
#define _TIMETICKS_H_

#include <boost/cstdint.hpp>

#include "value.hpp"
#include "exceptions.hpp"

using boost::uint32_t;

namespace agentxcpp
{
    /**
     * \brief Represents an TimeTicks as described in RFC 2741
     */
    class TimeTicks : public value_t
    {
	public:
            /**
	     * \brief The TimeTicks value.
	     *
	     * According to RFC 2578, Counter32 is a non-negative 32-bit 
	     * number.
	     */
	    uint32_t value;

	    /**
	     * \brief Create an TimeTicks without initialization.
	     *
	     * \param initial_value The initial value of the object.
	     */
	    TimeTicks(uint32_t initial_value = 0)
	    : value(initial_value)
	    {
	    }

	    /**
	     * \internal
	     *
	     * \brief Parse Constructor.
	     *
	     * This constructor parses the serialized form of the object.
	     * It takes an iterator, starts parsing at the position of the 
	     * iterator and advances the iterator to the position right behind 
	     * the object.
	     * 
	     * The constructor expects valid data from the stream; if parsing 
	     * fails, parse_error is thrown. In this case, the iterator 
	     * position is undefined.
	     *
	     * \param pos Iterator pointing to the current stream position.
	     *            The iterator is advanced while reading the header.
	     *
	     * \param end Iterator pointing one element past the end of the
	     *            current stream. This is needed to mark the end of the 
	     *            buffer.
	     *
	     * \param big_endian Whether the input stream is in big endian
	     *                   format
	     */
	    TimeTicks(binary::const_iterator& pos,
		      const binary::const_iterator& end,
		      bool big_endian=true);
	    
	    /**
	     * \internal
	     *
	     * \brief Encode the object as described in RFC 2741, section 5.4
	     *
	     * This function uses big endian.
	     */
	    virtual binary serialize() const;
    };
}

#endif
