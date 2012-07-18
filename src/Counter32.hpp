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

#ifndef _COUNTER32_H_
#define _COUNTER32_H_

#include <boost/cstdint.hpp>

#include "variable.hpp"
#include "exceptions.hpp"

using boost::uint32_t;

namespace agentxcpp
{
    /**
     * \brief Represents an Counter32 as descibed in RFC 2741
     */
     // TODO: RFC2578?
    class Counter32 : public variable
    {
	private:
	    /**
	     * \brief The counter value.
	     *
	     * According to RFC 2578, Counter32 is a non-negative 32-bit 
	     * number.
	     */
	    uint32_t value;

	public:
	    /**
             * \internal
             *
	     * \brief Create a counter without initialization.
	     *
	     * The value after creation is undefined.
	     */
	    Counter32() {}

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
	    Counter32(binary::const_iterator& pos,
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

	    /**
             * \internal
             *
	     * \brief Update the internal state of the object.
	     *
	     * This function calls get() to obtain a new value and writes that 
	     * value to the 'value' member.
	     *
             * \exception generic_error If obtaining the new value failed.
	     */
	    virtual void update()
	    {
		value = this->get();
	    }

            /**
             * \brief Obtain the current value for the object.
             *
             * This member function is derived by classes representing SNMP 
             * variables and shall return the current value of the object.
             *
             * The default implementation throws generic_error.
             *
             * \return The current value of the object.
             *
             * \exception generic_error If obtaining the current value fails.
             *                          No other exception shall be thrown.
             */
	    virtual uint32_t get()
	    {
		throw( generic_error() );
	    }

    };
}

#endif
