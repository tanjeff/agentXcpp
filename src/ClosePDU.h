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

#ifndef __CLOSEPDU_H__
#define __CLOSEPDU_H__

#include "exceptions.h"
#include "types.h"
#include "PDU.h"

namespace agentx
{
    class ClosePDU : public PDU
    {
	public:
	    
	    enum reason_t
	    {
		reasonOther = 1,
		reasonParseError = 2,
		reasonProtocolError = 3,
		reasonTimeouts = 4,
		reasonShutdown = 5,
		reasonByManager = 6
	    };
	    
	private:
	    /**
	     * \brief The reason why the session is being closed
	     */
	    reason_t reason;

	    /**
	     * \brief Hide default constructor
	     */
	    ClosePDU();

	public:
	    /**
	     * \brief Constructor
	     *
	     * \param reason The reason to session closure. Must be a valid
	     *               reason.
	     *
	     * \exception inval_param If the reason value is invalid.
	     */
	    ClosePDU(uint32_t sessionID,
		     reason_t reason,
		     Octet_String* context=0) throw(inval_param);

	    /**
	     * \brief Get the reason
	     */
	    reason_t get_reason() { return reason; }

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
	    ClosePDU(data_t::const_iterator& pos, bool big_endian) throw(parse_error);


	    /**
	     * \brief Serialize the %PDU
	     */
	    data_t serialize();
    };
}

#endif

