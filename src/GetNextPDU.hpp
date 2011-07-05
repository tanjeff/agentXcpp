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
#ifndef __GETNEXTPDU_H__
#define __GETNEXTPDU_H__

#include <vector>
#include <utility>

using std::vector;
using std::pair;

#include "PDU.hpp"
#include "oid.hpp"

namespace agentxcpp
{
    /**
     * \internal
     *
     * \brief Represents an GetNext-PDU
     */
    class GetNextPDU : public PDU
    {
	private:
	    /**
	     * \brief The SearchRange list
	     *
	     * A GetNext %PDU can have multiple SearchRanges. A SearchRange 
	     * consists of two oid's. We store them in a pair<>, and store the 
	     * pais<>'s in a vector< pair<> >.
	     */
	    vector< pair<oid,oid> > sr;

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
	     * \param end Iterator pointing one element past the end of the
	     *            current stream. This is needed to mark the end of the 
	     *            buffer.
	     *
	     * \param big_endian Whether the serialized form of the %PDU is
	     *                   in big_endian format.
	     *
	     * \exception parse_error If parsing fails, for example because
	     *			      reading the stream fails or the %PDU is 
	     *			      malformed.
	     */
	    GetNextPDU(data_t::const_iterator& pos,
		       const data_t::const_iterator& end,
		       bool big_endian);

	    /**
	     * \brief Default Constructor
	     *
	     * Sets the state of the object to the defaults as set by the 
	     * PDU::PDU() constructor, and initializes sr to be empty.
	     */
	    GetNextPDU() { }
	    
	    /**
	     * \brief Get the SearchRange list
	     *
	     * This function returns a reference to the internal SearchRange 
	     * list, which can then be modified in place.
	     *
	     * \note There is no set_sr() function, because the SearchRange
	     *       list can be modified in place.
	     */
	    vector< pair<oid,oid> >& get_sr()
	    {
		return this->sr;
	    }

	    /**
	     * \brief Serialize the %PDU
	     */
	    data_t serialize();
    };
}


#endif
