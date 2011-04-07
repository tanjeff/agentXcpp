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

#ifndef __RESPONSEPDU_H__
#define __RESPONSEPDU_H__

#include <vector>
using std::vector;

#include "PDU.h"
#include "oid.h"
#include "Octet_String.h"
#include "varbind.h"


namespace agentxcpp
{
    /**
     * \internal
     *
     * \brief Represents an Response-PDU.
     *
     * A Response-PDU is transmitted as a response to another PDU. The meaning 
     * of the individual fields of the Response-PDU depends on the PDU type to 
     * which the Response-PDU is sent. See RFC 2741 for details.
     */
    class ResponsePDU : public PDU
    {
	public:
	    /**
	     * \brief The type of the error field of the Response-PDU.
	     */
	    enum error_t
	    {
		noAgentXError = 0,
		openFailed = 256,
		notOpen = 257,
		indexWrongType = 258,
		indexAlreadyAllocated = 259,
		indexNoneAvailable = 260,
		indexNotAllocated = 261,
		unsupportedContext = 262,
		duplicateRegistration = 263,
		unknownRegistration = 264,
		unknownAgentCaps = 265,
		parseError = 266,
		requestDenied = 267,
		processingError = 268
	    };

	private:
	    /**
	     * \brief the sysUpTime field.
	     *
	     * See RFC 2741, section 6.2.16 "The agentx-Response-PDU" for an 
	     * explanation.
	     */
	    uint32_t sysUpTime;

	    /**
	     * \brief the index field.
	     *
	     * See RFC 2741, section 6.2.16 "The agentx-Response-PDU" for an 
	     * explanation.
	     */
	    uint16_t index;

	    /**
	     * \brief the error field.
	     *
	     * See RFC 2741, section 6.2.16 "The agentx-Response-PDU" for an 
	     * explanation.
	     */
	    error_t error;

	    /**
	     * \brief The VarBindList, if any.
	     *
	     * This vector may be empty if the %PDU has no VarBinds.
	     */
	    vector<varbind> varbindlist;

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
	    ResponsePDU(data_t::const_iterator& pos, bool big_endian);

	    /**
	     * \brief Default Constructor.
	     *
	     * The Response-PDU is initialized as follows:
	     * - res.error is set to noAgentXError
	     * - res.index is set to 0
	     * - res.sysUpTime is set to 0
	     */
	    ResponsePDU();

	    /**
	     * \brief Set the error status.
	     *
	     * \param error The new error status.
	     *
	     * \exception inval_param If the error value is not allowed.
	     */
	    void set_error(error_t error);

	    /**
	     * \brief Get the error status.
	     */
	    error_t get_error()
	    {
		return error;
	    }

	    /**
	     * \brief Set the sysUpTime.
	     */
	    void set_sysUpTime(uint32_t time)
	    {
		this->sysUpTime = time;
	    }
	    /**
	     * \brief Get the sysUpTime.
	     */
	    uint32_t get_sysUpTime()
	    {
		return sysUpTime;
	    }

	    /**
	     * \brief set the index value.
	     */
	    void set_index(uint16_t i)
	    {
		this->index = i;
	    }

	    /**
	     * \brief Get the index value
	     */
	    uint16_t get_index()
	    {
		return index;
	    }

	    /**
	     * \brief Set the VarBindList
	     */
	    void set_varbindlist(vector<varbind> varbindlist)
	    {
		this->varbindlist = varbindlist;
	    }
	    
	    /**
	     * \brief Get the VarBindList
	     */
	    vector<varbind> get_varbindlist()
	    {
		return this->varbindlist;
	    }

	    /**
	     * \brief Serialize the %PDU
	     */
	    data_t serialize();
    };
}


#endif
