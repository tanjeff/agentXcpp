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

#ifndef __CLOSEPDU_H__
#define __CLOSEPDU_H__

#include "exceptions.hpp"
#include "types.hpp"
#include "PDU.hpp"

namespace agentxcpp
{
    /**
     * \internal
     *
     * \brief This class represents a Close-PDU.
     */
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

	public:
	    /**
	     * \brief Default constructor
	     *
	     * Sets the state of the object to the defaults as set by the 
	     * PDU::PDU() constructor, plus the following:
	     * - reason = reasonOther
	     */
	    ClosePDU();

	    /**
	     * \brief Constructor
	     *
	     * \param reason The reason to session closure. Must be a valid
	     *               reason.
	     *
	     * \param sessionID The session ID of the PDU
	     *
	     * \exception inval_param If the reason value is invalid.
	     */
	    ClosePDU(uint32_t sessionID,
		     reason_t reason);

	    /**
	     * \brief Get the reason
	     */
	    reason_t get_reason()
	    {
		return reason;
	    }
	    /**
	     * \brief Set the reason
	     */
	    void set_reason(reason_t reason)
	    {
		this->reason = reason;
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
	    ClosePDU(data_t::const_iterator& pos,
		     const data_t::const_iterator& end,
		     bool big_endian);


	    /**
	     * \brief Serialize the %PDU
	     */
	    data_t serialize() const;
    };
}

#endif

