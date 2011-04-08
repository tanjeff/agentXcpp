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
#ifndef _OPAQUE_H_
#define _OPAQUE_H_

#include "types.h"
#include "variable.h"
#include "exceptions.h"

namespace agentxcpp
{
    /**
     * \brief Represents an Opaque obejct as descibed in RFC 2741, section 
     * 5.4
     */
    class Opaque : public variable
    {
	protected:
	    // We use data_t here although it was not invented for this.
	    // TODO: Is this a problem?
	    data_t value;

	public:
	    /**
	     * \internal
	     *
	     * \brief Encode the object as described in RFC 2741, section 5.4 / 
	     * 5.3
	     *
	     * Note:
	     * We always use big endian.
	     */
	    data_t serialize() const;
	    
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
	     * \param pos iterator pointing into the stream
	     * \param big_endian Whether the input stream is in big endian
	     *                   format
	     */
	    Opaque(data_t::const_iterator& pos, bool big_endian=true);

	    Opaque(data_t initial_value) : value(initial_value) {}

	    Opaque() { }
    };
}

#endif
