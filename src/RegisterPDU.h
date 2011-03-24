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
#ifndef __REGISTERPDU_H__
#define __REGISTERPDU_H__

#include "types.h"
#include "oid.h"
#include "PDU.h"

namespace agentxcpp
{
    class RegisterPDU : public PDU
    {
	private:
	    byte_t timeout;
	    byte_t priority;
	    byte_t range_subid;
	    oid subtree;
	    oid upper_bound;

	public:
	    /**
	     * \brief Parse constructor
	     *
	     * Construct the object by parsing a stream. See \ref parsing for 
	     * details about %PDU parsing.
	     *
	     * \param pos Iterator pointing to the current stream position.
	     *            The iterator is advanced while reading the header.
	     *
	     * \param big_endian Whether the serialized form of the %PDU is
	     *                   in big_endian format.
	     *
	     * \exception parse_error If parsing fails, for example because
	     *			      reading the stream fails or the %PDU is 
	     *			      malformed.
	     */
	    RegisterPDU(data_t::const_iterator& pos, bool big_endian) throw(parse_error);
	    
	    /**
	     * \brief Serialize the %PDU
	     */
	    data_t serialize();
    };
}
#endif
