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
#ifndef _NOTIFYPDU_H_
#define _NOTIFYPDU_H_

#include <vector>

#include <boost/cstdint.hpp>

#include "PDUwithContext.hpp"
#include "varbind.hpp"

using boost::uint32_t;
using std::vector;

namespace agentxcpp
{
    /**
     * \internal
     *
     * \brief Represents an Notify-PDU
     */
    class NotifyPDU : public PDUwithContext
    {
	private:
	    /**
	     * \brief The VarBind list
	     */
	    vector<varbind> vb;

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
	    NotifyPDU(binary::const_iterator& pos,
		      const binary::const_iterator& end,
		      bool big_endian);

	    /**
	     * \brief Default Constructor
	     *
	     * Sets the state of the object to the defaults as set by the 
	     * PDU::PDUwithContext() constructor, and initializes vb to be 
	     * empty.
	     */
	    NotifyPDU() { }
	    
	    /**
	     * \brief Get the VarBind list
	     *
	     * This function returns a reference to the internal VarBind list, 
	     * which can then be modified in place.
	     *
	     * \note
	     *       - There is no set_vb() function, because the VarBind
	     *       list can be modified in place.
	     *       - The following restrictions are placed on the vb's 
	     *       contents according to RFC 2741, 6.2.10.  "The 
	     *       agentx-Notify-PDU":
	     *         - If the subagent supplies sysUpTime.0, it must be 
	     *         present as the first varbind
	     *         - snmpTrapOID.0 must be present, as the second varbind 
	     *         if sysUpTime.0 was supplied, as the first if it was 
	     *         not.
	     */
	    vector<varbind>& get_vb()
	    {
		return this->vb;
	    }

	    /**
	     * \brief Serialize the %PDU
	     */
	    virtual binary serialize() const;
    };
}

#endif
