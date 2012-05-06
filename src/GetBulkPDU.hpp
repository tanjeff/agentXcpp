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
#ifndef _GETBULKPDU_H_
#define _GETBULKPDU_H_

#include <vector>
#include <utility>

#include <boost/cstdint.hpp>

#include "PDUwithContext.hpp"
#include "oid.hpp"

using std::vector;
using std::pair;
using boost::uint16_t;

namespace agentxcpp
{
    /**
     * \internal
     *
     * \brief Represents an GetBulk-PDU
     */
    class GetBulkPDU : public PDUwithContext
    {
	private:
	    /**
	     * \brief The SearchRange list
	     *
	     * A GetBulk %PDU can have multiple SearchRanges. A SearchRange 
	     * consists of two oid's. We store them in a pair<>, and store the 
	     * pais<>'s in a vector< pair<> >.
	     */
	    vector< pair<oid,oid> > sr;

	    /**
	     * \brief The number of variables in the SearchRangeList that are
	     *        not repeaters.
	     */
	    uint16_t non_repeaters;

	    /**
	     * \brief The maximum number of repetitions requested for repeating
	     *        variables.
	     */
	    uint16_t max_repititions;

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
	    GetBulkPDU(binary::const_iterator& pos,
		       const binary::const_iterator& end,
		       bool big_endian);

	    /**
	     * \brief Default Constructor
	     *
	     * Sets the state of the object to the defaults as set by the 
	     * PDU::PDU() constructor, sets non_repeaters to 0, max_repititions 
	     * to 0 and initializes sr to be empty.
	     */
	    GetBulkPDU()
	    {
		non_repeaters = max_repititions = 0;
	    }
	    
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
	     * \brief Get non_repeaters field
	     */
	    uint16_t get_non_repeaters()
	    {
		return non_repeaters;
	    }
	    
	    /**
	     * \brief Set non_repeaters field
	     */
	    void set_non_repeaters(uint16_t value)
	    {
		non_repeaters = value;
	    }
	    
	    /**
	     * \brief Get max_repititions field
	     */
	    uint16_t get_max_repititions()
	    {
		return max_repititions;
	    }
	    
	    /**
	     * \brief Set max_repititions field
	     */
	    void set_max_repititions(uint16_t value)
	    {
		max_repititions = value;
	    }

	    /**
	     * \brief Serialize the %PDU
	     */
	    virtual binary serialize() const;
    };
}


#endif
