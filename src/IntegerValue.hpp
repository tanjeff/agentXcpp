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

#ifndef _INTEGERVALUE_H_
#define _INTEGERVALUE_H_

#include <QtGlobal>

#include "AbstractValue.hpp"
#include "exceptions.hpp"
#include "OidValue.hpp"

namespace agentxcpp
{
    /**
     * \brief Represents an Integer as described in RFC 2741
     */
    class IntegerValue : public AbstractValue
    {
	public:
	    /**
	     * \brief The Integer value.
	     *
	     * According to RFC 2578, INTEGER is a signed 32-bit number.
	     */
	    qint32 value;

	    /**
             * \internal
             *
	     * \brief Create an IntegerValue object.
	     *
	     * The default value of the new object is 0.
	     *
	     * \exception None.
	     */
	    IntegerValue(qint32 _value=0) :value(_value) {}

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
	    IntegerValue(binary::const_iterator& pos,
		    const binary::const_iterator& end,
		    bool big_endian=true);

	    /**
	     * \brief Convert the value to an OID.
	     *
	     * The conversion is done according to RFC 2578,
	     * 7.7. "Mapping of the INDEX clause". The value is
	     * converted to an Oid with a single subid. Note that
	     * INTEGER values are signed, while subids are not.
	     * A negative value with be converted to big unsigned
	     * subid.
	     *
	     * \note If an INTEGER is used in an INDEX clause, the
	     *       value 0 should be avoided according to
	     *       RFC 2578, 7.7. "Mapping of the INDEX clause".
	     */
	    OidValue toOid() const
	    {
	        OidValue oid;
	        oid.push_back(value);
	        return oid;
	    }
    };
}

#endif
