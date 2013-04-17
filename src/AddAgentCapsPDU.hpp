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
#ifndef _ADDAGENTCAPSPDU_H_
#define _ADDAGENTCAPSPDU_H_

#include "PDUwithContext.hpp"
#include "OidValue.hpp"
#include "OctetStringValue.hpp"

namespace agentxcpp
{
    /**
     * \internal
     *
     * \brief Represents an AddAgentCaps-PDU
     */
    class AddAgentCapsPDU : public PDUwithContext
    {
	private:
	    /**
	     * \brief The Object Identifier containing the value of an
	     *        invocation of the AGENT-CAPABILITIES macro.
	     *
	     * See RFC 2741, 6.2.14. "The agentx-AddAgentCaps-PDU" for 
	     * details.
	     */
	    OidValue id;

	    /**
	     * \brief An Octet String containing a DisplayString to be used as
	     *        the value of sysORDescr corresponding to the sysORID 
	     *        value above.
	     *
	     * See RFC 2741, 6.2.14. "The agentx-AddAgentCaps-PDU" for details.
	     */
	    OctetStringValue descr;
	    

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
	    AddAgentCapsPDU(binary::const_iterator& pos,
			    const binary::const_iterator& end,
			    bool big_endian);

	    /**
	     * \brief Default Constructor
	     *
	     * Sets the state of the object to the defaults as set by the 
	     * PDU::PDUwithContext() constructor. The fields 'id' and 'descr' 
	     * are initialized to be empty.
	     */
	    AddAgentCapsPDU() { }

	    /**
	     * \brief Constructor
	     *
	     * Sets the state of the object to the defaults as set by the 
	     * PDU::PDUwithContext() constructor. The fields id and descr are 
	     * initialized according to the parameters.
	     *
	     * \param id The value of the id field.
	     *
	     * \param descr The value of the descr field.
	     */
	    AddAgentCapsPDU(OidValue id, OctetStringValue descr);

	    /**
	     * \brief Set the id field
	     *
	     * \param id The new id value.
	     */
	    void set_id(OidValue id)
	    {
		this->id = id;
	    }
	    
	    /**
	     * \brief Get the id field
	     *
	     * \return The id value.
	     */
	    OidValue get_id()
	    {
		return this->id;
	    }
	    
	    /**
	     * \brief Set the descr field
	     *
	     * \param descr The new descr value.
	     */
	    void set_descr(OctetStringValue descr)
	    {
		this->descr = descr;
	    }
	    
	    /**
	     * \brief Get the descr field
	     *
	     * \return The descr value.
	     */
	    OctetStringValue get_descr()
	    {
		return this->descr;
	    }

	    /**
	     * \brief Serialize the %PDU
	     */
	    virtual binary serialize();
    };
}

#endif
