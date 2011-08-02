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
#ifndef __INDEXDEALLOCATEPDU_H__
#define __INDEXDEALLOCATEPDU_H__

#include <vector>
using std::vector;

#include "PDUwithContext.hpp"
#include "varbind.hpp"

namespace agentxcpp
{
    /**
     * \internal
     *
     * \brief Represents a IndexDeallocate-PDU
     */
    class IndexDeallocatePDU : public PDUwithContext
    {
	private:
	    /**
	     * \brief The VarBind list containing the index names and values to
	     *        be released.
	     */
	    vector<varbind> vb;
	
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
	    IndexDeallocatePDU(data_t::const_iterator& pos,
			       const data_t::const_iterator& end,
			       bool big_endian);

	    /**
	     * \brief Default Constructor
	     *
	     * Sets the state of the object to the defaults as set by the 
	     * PDU::PDUwithContext() constructor and initializes vb to be 
	     * empty.
	     */
	    IndexDeallocatePDU() { }
	    
	    /**
	     * \brief Get the VarBind list
	     *
	     * This function returns a reference to the internal VarBind list, 
	     * which can then be modified in place.
	     *
	     * \note There is no set_vb() function, because the VarBind
	     *       list can be modified in place.
	     */
	    vector<varbind>& get_vb()
	    {
		return this->vb;
	    }
	    
	    /**
	     * \brief Serialize the %PDU
	     */
	    data_t serialize();
    };
}

#endif
