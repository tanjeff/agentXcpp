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

#ifndef _INTEGER_H_
#define _INTEGER_H_

#include "types.hpp"
#include "variable.hpp"
#include "exceptions.hpp"

namespace agentxcpp
{
    /**
     * \brief Represents an Integer as descibed in RFC 2741
     */
    class Integer : public variable
    {
	protected:
	    /**
	     * \brief The Integer value
	     */
	    uint32_t value;

	public:
	    /**
	     * \brief Create an Integer object
	     *
	     * The default value of the new object is 0.
	     *
	     * \exception None.
	     */
	    Integer(uint32_t _value=0) :value(_value) {}

	    /**
	     * \brief Get the value
	     */
	    uint32_t get_value()
	    {
		return value;
	    }
	    
	    /**
	     * \brief Set the value
	     */
	    void set_value(uint32_t v)
	    {
		value = v;
	    }
	    
	    /**
	     * \internal
	     *
	     * \brief Encode the object as described in RFC 2741, section 5.4
	     *
	     * This function uses big endian.
	     */
	    virtual data_t serialize() const;
	    
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
	    Integer(data_t::const_iterator& pos,
		    const data_t::const_iterator& end,
		    bool big_endian=true);

	    /**
	     * \brief Update the internal state of the object.
	     *
	     * This function calls get() to obtain a new value and writes that 
	     * value to the 'value' member.
	     *
             * \exception generic_error If obtaining the new value failed.
	     */
	    virtual void update()
	    {
		//value = this->get();
	    }
    };
}

#endif
