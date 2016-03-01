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

#ifndef _PDUWITHCONTEXT_H_
#define _PDUWITHCONTEXT_H_


#include "PDU.hpp"
#include "OctetStringVariable.hpp"


namespace agentxcpp
{
    /**
     * \internal
     *
     * This class is never instantiated itself, but serves as the base class 
     * for concrete %PDU classes which have a context field (e.g.  GetPDU).  
     * For concrete %PDU's without context field, the class PDUwithContext is 
     * used.
     *
     * This derives from the PDU class and adds a context field. This field is 
     * only meaningful if PUD::non_default_context is true. If 
     * non_default_context is false, the context will neither be read in when a 
     * PDU is parsed nor will it be serialized by 
     * PDUwithContext::add_header().
     */
    class PDUwithContext : public PDU
    {
	private:
	    /**
	     * \brief The context
	     */
	    OctetStringVariable context;

	protected:
	    /**
	     * \brief Parse constructor
	     *
	     * Read the context from a buffer and let the PDU::PDU() parse 
	     * constructor do the rest. Is called by the parse constructors of 
	     * derived classes. See \ref parsing for details about %PDU 
	     * parsing.
	     * 
	     * \param pos Iterator pointing to the current stream position. The
	     *		  iterator is advanced while reading the header.
	     *
	     * \param end Iterator pointing one element past the end of the
	     *            current stream. This is needed to mark the end of the 
	     *            buffer.
	     *
	     * \param big_endian Whether the serialized form of the %PDU is in
	     *                   big_endian format.
	     *
	     * \exception parse_error If parsing fails, for example because
	     *			      reading the stream fails or the %PDU is 
	     *			      malformed.
	     */
	    PDUwithContext(binary::const_iterator& pos,
			   const binary::const_iterator& end,
			   bool big_endian)
		: PDU(pos, end, big_endian)
	    {
		// read the context (if present)
		if( non_default_context )
		{
		    this->context = OctetStringVariable(pos, end, big_endian);
		}
	    }
	    
	    /**
	     * \brief Add PDU header and context field to the payload
	     *
	     * Add the PDU header and the context field to the payload. Called 
	     * by derived classes during serialization.
	     * 
	     * \warning The payload must not grow or shrink after a call to
	     *          this function as its size is encoded into the header.
	     * \warning The function must not be called twice for the same PDU,
	     *          because this would add two headers and thus generate a 
	     *          malformed PDU.
	     *
	     * Header and context are encoded in big endian format.
	     *
	     * \param type The PDU type, according to RFC 2741, 6.1. "AgentX
	     *             PDU Header".
	     *
	     * \param payload The payload of the PDU. The header is added to
	     *                the payload, i.e. payload is altered by this 
	     *                function.
	     */
	    void add_header(type_t type, binary& payload) const
	    {
		// Prepend context to payload, if present
		if( non_default_context )
		{
		    payload.insert(0, context.serialize());
		}

		// Add header
		PDU::add_header(type, payload);
	    }

	    /**
	     * \brief Default Constructor
	     *
	     * Sets the state of the object to the defaults as set by the 
	     * PDU::PDU() constructor. The non_default_context flag is set to 
	     * false by default, which means that the object has no context.
	     */
	    PDUwithContext() {}
	    
	public:
	    /**
	     * \brief Whether the PDU has a context
	     */
	    bool has_context()
	    {
		return this->non_default_context;
	    }
	    
	    /**
	     * \brief Get context
	     *
	     * \return The internally stored context.
	     *
	     * \warning The returned context is only valid if the PDU actually
	     *          has a context. This can be determined using 
	     *          has_context().
	     */
	    OctetStringVariable get_context()
	    {
		return this->context;
	    }
	    
	    /**
	     * \brief Set the PDU's context
	     *
	     * \param value The new context. The given value may be the empty
	     *              string, which does \em not delete the context, but 
	     *              set the empty string as context. To remove the 
	     *              context from the PDU, use remove_context().
	     */
	    void set_context(OctetStringVariable value)
	    {
		this->context = value;
		this->non_default_context = true;
	    }

	    /**
	     * \brief Removes the context from the PDU.
	     *
	     * Afterwards the PDU has no context any longer.
	     */
	    void remove_context()
	    {
		this->non_default_context = false;
	    }

    };
}

#endif

