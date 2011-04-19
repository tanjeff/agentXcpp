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

#ifndef _OCTET_STRING_H_
#define _OCTET_STRING_H_

#include "types.h"
#include "variable.h"
#include "exceptions.h"

namespace agentxcpp
{
    /**
     * \brief Represents an Octet String as descibed in RFC 2741, section 5.3
     */
    class Octet_String : public variable
    {
	protected:
	    // We use data_t here although it was not invented for this.
	    // TODO: Is this a problem?
	    data_t value;

	public:
	    /**
	     * \internal
	     *
	     * \brief Encode the object as described in RFC 2741, section 5.3
	     *
	     * Note:
	     * We always use big endian.
	     */
	    data_t serialize() const;

	    /**
	     * \brief FIXME
	     */
	    Octet_String(data_t initial_value) : value(initial_value) {}

	    Octet_String(std::string initial_value);

	    /**
	     * \brief FIXME
	     */
	    Octet_String() { }
	    
	    /**
	     * \internal
	     *
	     * \brief Construct the object from input stream
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
	    Octet_String(data_t::const_iterator& pos,
		         const data_t::const_iterator& end,
			 bool big_endian=true);

	    /**
	     * \brief Set the current value
	     */
	    void set_value(data_t new_value) { value = new_value; }

	    /**
	     * \brief get the current value
	     */
	    data_t get_value() { return value; }
    };
}

#endif
