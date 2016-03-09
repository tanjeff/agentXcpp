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

#ifndef _RESPONSEPDU_H_
#define _RESPONSEPDU_H_

#include <vector>

#include <QtGlobal>

#include "PDU.hpp"
#include "OctetStringVariable.hpp"
#include "Varbind.hpp"

using std::vector;


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
                // Values from RFC 2741, 6.2.16 "The agentx-Response-PDU"
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
		processingError = 268,

                // Values from RFC 1905 (allowed according to RFC 2741, 6.2.16 
                // "The agentx-Response-PDU")
                tooBig = 1,
                noSuchName = 2,   // for proxy compatibility
                badValue = 3,     // for proxy compatibility
                readOnly = 4,     // for proxy compatibility
                genErr = 5,
                noAccess = 6,
                wrongType = 7,
                wrongLength = 8,
                wrongEncoding = 9,
                wrongValue = 10,
                noCreation = 11,
                inconsistentValue = 12,
                resourceUnavailable = 13,
                commitFailed = 14,
                undoFailed = 15,
                authorizationError = 16,
                notWritable = 17,
                inconsistentName = 18
	    };

	private:
	    /**
	     * \brief the sysUpTime field.
	     *
	     * See RFC 2741, section 6.2.16 "The agentx-Response-PDU" for an 
	     * explanation.
	     */
	    quint32 sysUpTime;

	    /**
	     * \brief the index field.
	     *
	     * See RFC 2741, section 6.2.16 "The agentx-Response-PDU" for an 
	     * explanation.
	     */
	    quint16 index;

	    /**
	     * \brief the error field.
	     *
	     * See RFC 2741, section 6.2.16 "The agentx-Response-PDU" for an 
	     * explanation.
	     */
	    error_t error;

	public:
	    /**
	     * \brief The VarBindList.
	     *
	     * This vector may be empty if the %PDU has no VarBinds.
	     */
	    vector<Varbind> varbindlist;

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
	    ResponsePDU(binary::const_iterator& pos,
			const binary::const_iterator& end,
			bool big_endian);

	    /**
	     * \brief Default Constructor.
	     *
	     * Sets the state of the object to the defaults as set by the 
	     * PDU::PDU() constructor, plus the following:
	     * - sysUpTime = 0
	     * - error = noAgentXError
	     * - index = 0
	     * - varbindlist = \<empty\>
	     */
	    ResponsePDU();

	    /**
	     * \brief Set the error status.
	     *
	     * \param error The new error status.
	     *
	     * \exception None.
	     */
	    void set_error(error_t error)
	    {
	        this->error = error;
	    }

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
	    void set_sysUpTime(quint32 time)
	    {
		this->sysUpTime = time;
	    }
	    /**
	     * \brief Get the sysUpTime.
	     */
	    quint32 get_sysUpTime()
	    {
		return sysUpTime;
	    }

	    /**
	     * \brief set the index value.
	     */
	    void set_index(quint16 i)
	    {
		this->index = i;
	    }

	    /**
	     * \brief Get the index value
	     */
	    quint16 get_index()
	    {
		return index;
	    }

	    /**
	     * \brief Serialize the %PDU
	     */
	    binary serialize() const;
    };
}


#endif
