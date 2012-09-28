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
#ifndef _OPAQUE_H_
#define _OPAQUE_H_

#include "variable.hpp"
#include "exceptions.hpp"

namespace agentxcpp
{
    /**
     * \brief Represents an Opaque obejct as descibed in RFC 2741, section 
     * 5.4
     */
    class Opaque : public variable
    {
	private:
	    /**
	     * \brief The string.
	     *
	     * According to RFC 2578, Opaque is used to wrap any ASN.1-type.
	     */
	    binary value;

	public:
	    /**
	     * \internal
	     *
	     * \brief Encode the object as described in RFC 2741, section 5.4 / 
	     * 5.3
	     *
	     * Note:
	     * We always use big endian.
	     */
	    binary serialize() const;
	    
	    /**
	     * \internal
	     *
	     * \brief Construct the object from input stream
	     *
	     * This constructor parses the serialized form of the object.
	     * It takes an iterator, starts parsing at the position of the 
	     * iterator and advances the iterator to the position right behind 
	     * the object.
	     * 
	     * The constructor expects valid data from the stream; if parsing 
	     * fails, parse_error is thrown. In this case, the iterator 
	     * position is undefined.
	     *
	     * \param pos Iterator pointing to the current stream position.
	     *            The iterator is advanced while reading the header.
	     *
	     * \param end Iterator pointing one element past the end of the
	     *            current stream. This is needed to mark the end of the 
	     *            buffer.
	     *
	     * \param big_endian Whether the input stream is in big endian
	     *                   format
	     */
	    Opaque(binary::const_iterator& pos,
		   const binary::const_iterator& end,
		   bool big_endian=true);

            /**
             * \internal
             *
             * \brief Constructor for initializing with data.
             */
	    Opaque(binary initial_value) : value(initial_value) {}

            /**
             * \internal
             *
	     * \brief Update the internal state of the object.
	     *
	     * This function calls get() to obtain a new value and writes that 
	     * value to the 'value' member.
	     *
             * \exception generic_error If obtaining the new value failed.
	     */
	    virtual void update()
	    {
		value = this->get();
	    }

            /**
             * \brief Perform TestSet operation.
             *
             * This member function can be overriden by classes representing 
             * SNMP variables.  It shall validate that a Set operation on this 
             * object would succeed and should allocate all resources which are 
             * necessary to perform the Set operation.  Later on, either 
             * cleanupset() or undoset() will be called to deallocate the 
             * resources again.
             *
             * The function shall return an error code conforming to the 
             * documentation of the testset_result_t enumeration.
             *
             * The default behaviour is to return noAccess, indicating that the 
             * variable is read-only. Thus, implementation of a read-only 
             * variable need not to override this function.
             * 
             * \return A value as described in the testset_result_t
             *         documentation.
             */
            virtual testset_result_t testset()
            {
                return noAccess;
            }

            /**
             * \brief Obtain the current value for the object.
             *
             * This member function is derived by classes representing SNMP 
             * variables and shall return the current value of the object.
             *
             * The default implementation throws generic_error.
             *
             * \return The current value of the object.
             *
             * \exception generic_error If obtaining the current value fails.
             *                          No other exception shall be thrown.
             */
	    virtual binary get()
	    {
		throw( generic_error() );
	    }
    };
}

#endif
