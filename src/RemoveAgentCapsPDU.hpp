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
#ifndef _REMOVEAGENTCAPSPDU_H_
#define _REMOVEAGENTCAPSPDU_H_

#include "PDUwithContext.hpp"
#include "oid.hpp"

namespace agentxcpp
{
    /**
     * \internal
     *
     * \brief Represents an RemoveAgentCaps-PDU
     */
    class RemoveAgentCapsPDU : public PDUwithContext
    {
	private:
	    /**
	     * \brief The Object Identifier containing the value of sysORID 
	     * that should no longer be exported.
	     *
	     * See RFC 2741, 6.2.15. "The agentx-RemoveAgentCaps-PDU" for 
	     * details.
	     */
	    oid id;


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
	    RemoveAgentCapsPDU(data_t::const_iterator& pos,
			    const data_t::const_iterator& end,
			    bool big_endian);

	    /**
	     * \brief Default Constructor
	     *
	     * Sets the state of the object to the defaults as set by the 
	     * PDU::PDUwithContext() constructor. The field 'id' is initialized 
	     * to be empty.
	     */
	    RemoveAgentCapsPDU() { }

	    /**
	     * \brief Constructor
	     *
	     * Sets the state of the object to the defaults as set by the 
	     * PDU::PDUwithContext() constructor. The field id is initialized 
	     * according to the parameter.
	     *
	     * \param id The value of the id field.
	     */
	    RemoveAgentCapsPDU(oid id);

	    /**
	     * \brief Set the id field
	     *
	     * \param id The new id value.
	     */
	    void set_id(oid id)
	    {
		this->id = id;
	    }
	    
	    /**
	     * \brief Get the id field
	     *
	     * \return The id value.
	     */
	    oid get_id()
	    {
		return this->id;
	    }
	    
	    /**
	     * \brief Serialize the %PDU
	     */
	    data_t serialize();
    };
}

#endif
