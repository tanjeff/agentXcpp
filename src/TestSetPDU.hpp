/*
 * Copyright 2011-2016 Tanjeff-Nicolai Moos <tanjeff@cccmz.de>
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
#ifndef _TESTSETPDU_H_
#define _TESTSETPDU_H_

#include <vector>

#include "PDUwithContext.hpp"
#include "Varbind.hpp"

using std::vector;

namespace agentxcpp
{
    /**
     * \internal
     *
     * \brief Represents an TestSet-PDU
     */
    class TestSetPDU : public PDUwithContext
    {
	private:
	    /**
	     * \brief The VarBind list
	     */
	    // TODO: should be use std::list instead? After all, ist is called 
	    // a VarBind _list_. This is also true for some other PDU's, grep 
	    // for 'vector' in *PDU.hpp
	    vector<Varbind> vb;

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
	    TestSetPDU(binary::const_iterator& pos,
		       const binary::const_iterator& end,
		       bool big_endian);

	    /**
	     * \brief Default Constructor
	     *
	     * Sets the state of the object to the defaults as set by the 
	     * PDU::PDU() constructor, and initializes vb to be empty.
	     */
	    TestSetPDU() { }
	    
	    /**
	     * \brief Get the VarBind list
	     *
	     * This function returns a reference to the internal VarBind list, 
	     * which can then be modified in place.
	     *
	     * \note There is no set_vb() function, because the VarBind
	     *       list can be modified in place.
	     */
	    vector<Varbind>& get_vb()
	    {
		return this->vb;
	    }

	    /**
	     * \brief Serialize the %PDU
	     */
	    binary serialize() const;
    };
}


#endif
