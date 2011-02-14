/*
 * Copyright 2011 Tanjeff-Nicolai Moos <tanjeff@cccmz.de>
 *
 * This file is part of agentXcpp.
 *
 * AgentXcpp is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * AgentXcpp is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with AgentXcpp.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef _VARBIND_H_
#define _VARBIND_H_

#include <istream>

#include "types.h"
#include "oid.h"
#include "variable.h"

namespace agentx
{
    /**
     * \brief Represents a VarBind according to RFC 2741, section 5.4.
     */
    class varbind
    {
	private:
	    /**
	     * \brief The name (OID) of the VarBind.
	     */
	    oid* name;

	    /**
	     * \brief The variable inside the varbind.
	     *
	     * This pointer may be 0 if the varbind has a type without a 
	     * variable (e.g. "NoSuchObject").
	     */
	    variable* var;

	    /**
	     * \brief The type of the varbind.
	     *
	     * This field represents the type as described in RFC 2741, section 
	     * 5.4. The serialize() function will copy it directly into the 
	     * varbind.
	     */
	    uint16_t type;
	public:

	    /**
	     * \brief Create a VarBind with an oid and a var.
	     *
	     * The variable must be one of the following types:
	     * - Integer
	     * - Octet_String
	     * - oid
	     * - IpAddress
	     * - Counter32
	     * - Gauge32
	     * - TimeTicks
	     * - Opaque
	     * - Counter64
	     * If the type of the variable cannot be determined, inval_param is 
	     * thrown.
	     */
	    varbind(oid*, variable* v) throw(inval_param);
	    
	    /**
	     * \brief These values can be used to create a VarBind.
	     */
	    enum type_t {
		Null = 5,
		noSuchObject = 128,
		noSuchInstance = 129,
		endOfMibView = 130
	    };


	    /**
	     * \brief Create VarBind without var, but with a type.
	     *
	     * Only the constants defined by varbind::type_t are allowed.  A 
	     * wrong type will cause an inval_param exception.
	     */
	    varbind(oid*, type_t) throw(inval_param);

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
	    varbind(data_t::const_iterator& pos, bool big_endian=true) throw(parse_error);

	    /**
	     * \internal
	     *
	     * \brief Serialize the varbind.
	     *
	     * This creates the binary representation of the varbind.
	     */
	    data_t serialize();

    };

}
#endif
