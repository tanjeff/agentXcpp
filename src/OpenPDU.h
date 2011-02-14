/*
 * Copyright 2011 Tanjeff-Nicolai Moos <tanjeff@cccmz.de>
 *
 * This file is part of agentXcpp.
 *
 * AgentXcpp is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * AgentXcpp is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with AgentXcpp.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __OPENPDU_H__
#define __OPENPDU_H__

#include "PDU.h"
#include "oid.h"
#include "Octet_String.h"

namespace agentx
{
    /**
     * \internal
     *
     * \brief Represents an Open-PDU
     */
    class OpenPDU : public PDU
    {
	private:
	    byte_t timeout;
	    oid id;
	    Octet_String descr;

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
	    OpenPDU(data_t::const_iterator& pos, bool big_endian);

	    /**
	     * \brief Constructor
	     *
	     * \param description A string describing the subagent.
	     *
	     * \param timeout The length of time, in seconds, that a master
	     *		      agent should allow to elapse after dispatching a 
	     *		      message on a session before it regards the 
	     *		      subagent as not responding. This is the default 
	     *		      value for the session, and may be overridden by 
	     *		      values associated with specific registered MIB 
	     *		      regions.  The default value of 0 indicates that 
	     *		      there is no session-wide default value.
	     *
	     * \param ID An Object Identifier that identifies the subagent.
	     *           Default is an emtpy OID.
	     */
	    OpenPDU(Octet_String description=Octet_String(),
		    byte_t timeout=0,
		    oid ID=oid());

	    /**
	     * \brief Serialize the %PDU
	     */
	    data_t serialize();
    };
}


#endif
