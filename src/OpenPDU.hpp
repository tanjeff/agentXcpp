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
#ifndef _OPENPDU_H_
#define _OPENPDU_H_

#include "PDU.hpp"
#include "OidVariable.hpp"
#include "OctetStringVariable.hpp"

namespace agentxcpp
{
    /**
     * \internal
     *
     * \brief Represents an Open-PDU
     */
    class OpenPDU : public PDU
    {
	private:
	    quint8 timeout;
	    OidVariable id;
	    OctetStringVariable descr;

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
	    OpenPDU(binary::const_iterator& pos,
		    const binary::const_iterator& end,
		    bool big_endian);

	    /**
	     * \brief Default Constructor
	     *
	     * Sets the state of the object to the defaults as set by the 
	     * PDU::PDU() constructor, plus the following:
	     * - timeout = 0
	     * - oid = null Object Identifier
	     * - descr = empty string
	     */
	    OpenPDU();

	    /**
	     * \brief Get timeout
	     */
	    quint8 get_timeout()
	    {
		return this->timeout;
	    }

	    /**
	     * \brief Set timeout
	     *
	     * \param timeout The length of time, in seconds, that a master
	     *		      agent should allow to elapse after dispatching a 
	     *		      message on a session before it regards the 
	     *		      subagent as not responding. This is the default 
	     *		      value for the session, and may be overridden by 
	     *		      values associated with specific registered MIB 
	     *		      regions.  The default value of 0 indicates that 
	     *		      there is no session-wide default value.
	     */
	    void set_timeout(quint8 timeout)
	    {
		this->timeout = timeout;
	    }

	    /**
	     * \brief Get ID
	     */
	    OidVariable get_id()
	    {
		return this->id;
	    }

	    /**
	     * \brief Set ID
	     */
	    void set_id(OidVariable id)
	    {
		this->id = id;
	    }
	    
	    /**
	     * \brief Get descr
	     */
	    OctetStringVariable get_descr()
	    {
		return this->descr;
	    }

	    /**
	     * \brief Set descr
	     */
	    void set_descr(OctetStringVariable descr)
	    {
		this->descr = descr;
	    }


	    /**
	     * \brief Serialize the %PDU
	     */
	    binary serialize() const;
    };
}


#endif
