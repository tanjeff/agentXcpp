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
#ifndef _OPAQUE_H_
#define _OPAQUE_H_

#include "value.hpp"
#include "exceptions.hpp"

namespace agentxcpp
{
    /**
     * \brief Represents an Opaque object as described in RFC 2741, section
     * 5.4.
     *
     * This class represent binary data.
     *
     * \note There are no functions to convert the binary data to/from
     *       std::string. If that is needed, Octet_String might be a better
     *       choice.
     */
    class Opaque : public value_t
    {
	public:
	    /**
	     * \brief The string.
	     *
	     * According to RFC 2578, Opaque is used to wrap any ASN.1-type.
	     */
	    binary value;

	    /**
	     * \internal
	     *
	     * \brief Encode the object as described in RFC 2741, section 5.4 / 
	     * 5.3
	     *
	     * Note:
	     * We always use big endian.
	     */
	    binary serialize() const;
	    
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
	    Opaque(binary::const_iterator& pos,
		   const binary::const_iterator& end,
		   bool big_endian=true);

            /**
             * \brief Constructor for initializing with data.
             */
	    Opaque(binary initial_value) : value(initial_value) {}
    };
}

#endif
