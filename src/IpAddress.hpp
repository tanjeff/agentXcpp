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

#ifndef _IPADDRESS_H_
#define _IPADDRESS_H_

#include "types.hpp"
#include "variable.hpp"
#include "exceptions.hpp"

namespace agentxcpp
{
    /**
     * \brief Represents an IP address as descibed in RFC 2741, section 5.4
     */
    class IpAddress : public variable
    {
	private:
	    /**
	     * \brief Hide default constructor
	     */
	    IpAddress();

	protected:
	    uint32_t address[4]; // only IPv4

	public:
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
	    IpAddress(data_t::const_iterator& pos,
		      const data_t::const_iterator& end,
		      bool big_endian=true);

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
	     * \brief FIXME
	     */
	    IpAddress(uint32_t a,
		    uint32_t b,
		    uint32_t c,
		    uint32_t d)
	    {
		address[0] = a;
		address[1] = b;
		address[2] = c;
		address[3] = d;
	    }

	    /**
	     * \brief Update the internal state of the object.
	     *
	     * This function calls get() to obtain a new value and writes that 
	     * value to the 'address' member.
	     *
             * \exception generic_error If obtaining the new value failed.
	     */
	    virtual void update()
	    {
		//address = this->get();
	    }
    };
}

#endif
