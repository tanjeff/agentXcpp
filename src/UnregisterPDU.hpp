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
#ifndef _UNREGISTERPDU_H_
#define _UNREGISTERPDU_H_

#include <boost/cstdint.hpp>

#include "PDUwithContext.hpp"
#include "oid.hpp"

using boost::uint32_t;

namespace agentxcpp
{
    /**
     * \internal
     *
     * \brief This class represents an Unregister-PDU.
     */
    class UnregisterPDU : public PDUwithContext
    {
	private:
	    byte_t priority;
	    byte_t range_subid;
	    oid subtree;
	    uint32_t upper_bound;

	public:
	    /**
	     * \brief Set the upper_bound.
	     */
	    void set_upper_bound(uint32_t upper_bound)
	    {
		this->upper_bound = upper_bound;
	    }
	    /**
	     * \brief Get the upper_bound.
	     */
	    uint32_t get_upper_bound()
	    {
		return this->upper_bound;
	    }

	    /**
	     * \brief Set the subtree.
	     */
	    void set_subtree(oid subtree)
	    {
		this->subtree = subtree;
	    }
	    /**
	     * \brief Get the subtree.
	     */
	    oid get_subtree()
	    {
		return this->subtree;
	    }

	    /**
	     * \brief Set the range_subid.
	     */
	    void set_range_subid(byte_t range_subid)
	    {
		this->range_subid = range_subid;
	    }
	    /**
	     * \brief Get the range_subid.
	     */
	    byte_t get_range_subid()
	    {
		return this->range_subid;
	    }

	    /**
	     * \brief Set the priority.
	     */
	    void set_priority(byte_t priority)
	    {
		this->priority = priority;
	    }
	    /**
	     * \brief Get the priority.
	     */
	    byte_t get_priority()
	    {
		return this->priority;
	    }

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
	    UnregisterPDU(data_t::const_iterator& pos,
			  const data_t::const_iterator& end,
			  bool big_endian);
	    
	    /**
	     * \brief Serialize the %PDU
	     */
	    data_t serialize() const;
	    
	    /**
	     * \brief Default Constructor
	     *
	     * Sets the state of the object to the defaults as set by the 
	     * PDU::PDU() constructor, plus the following:
	     * - priority = 127 (The default when registering an object 
	     *   according to RFC 2741, section '6.2.3.  The   
	     *   agentx-Register-PDU')
	     * - subtree = null Object Identifier
	     * - range_subid = 0
	     * - upper_bound = null Object Identifier
	     */
	    UnregisterPDU();
    };
}
#endif
