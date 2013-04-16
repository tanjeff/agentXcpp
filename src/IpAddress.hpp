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

#ifndef _IPADDRESS_H_
#define _IPADDRESS_H_

#include <boost/cstdint.hpp>

#include "AbstractValue.hpp"
#include "exceptions.hpp"

using boost::uint32_t;

namespace agentxcpp
{
    /**
     * \brief Represents an IP address as described in RFC 2741, section 5.4
     */
    class IpAddress : public AbstractValue
    {
	private:
	    /**
	     * \brief Hide default constructor
	     */
	    IpAddress();

	    /**
	     * \brief The IP address.
	     *
	     * According to RFC 2578, IpAddress is a 32-bit number.
	     */
            uint8_t address[4]; // only IPv4

	public:
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
	    IpAddress(binary::const_iterator& pos,
		      const binary::const_iterator& end,
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
	    binary serialize() const;

	    /**
             * \brief Construct an IpAddress object.
             *
             * The stored IpAddress is initialized to "a.b.c.d".
             *
             * \exception None.
	     */
	    IpAddress(uint8_t a,
		      uint8_t b,
		      uint8_t c,
		      uint8_t d)
	    {
		address[0] = a;
		address[1] = b;
		address[2] = c;
		address[3] = d;
	    }

            /**
             * \brief Set the IpAddress.
             *
             * The stored IpAddress is updated to "a.b.c.d".
             *
             * \exception None.
             */
	    void set_value(uint8_t a,
                           uint8_t b,
                           uint8_t c,
                           uint8_t d)
            {
                address[0] = a;
                address[1] = b;
                address[2] = c;
                address[3] = d;
            }

	    /**
	     * \brief Access a component of the stored IpAddress.
	     *
	     * This function returns the component with the given index. Note
	     * that exactly 4 components are stored. The return value is a
	     * reference to the component, so that it can be read/written, in
	     * other words, the IpAddress object can be accessed like an
	     * ordinary array.
	     *
	     * \param index The index (value in the range 0...3).
	     *
	     * \return A reference to the component.
	     *
	     * \exception inval_param If the index is out of bound.
	     */
	    uint8_t& operator[](unsigned index)
	    {
	        if(index > 3)
	        {
	            throw(inval_param());
	        }
	        return address[index];
	    }
    };
}

#endif
