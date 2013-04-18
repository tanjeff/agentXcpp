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
#ifndef _VALUE_H_
#define _VALUE_H_

#include "binary.hpp"

namespace agentxcpp
{
    /**
     * \brief This class represents a value.
     *
     * This class is used as base class for the specific value types (such as
     * OctetStringValue or IntegerValue).
     */
    class AbstractValue
    {
	public:
	    /**
             * \internal
             *
	     * \brief Serialize the value.
	     *
	     * This function must be implemented by all derived classes. The 
	     * function shall generate a serialized form of the value.
	     *
	     * \return The serialized form of the value.
	     *
	     * \exception None: The function shall not throw.
	     */
	    virtual binary serialize() const = 0;

            /**
             * \brief Destructor.
             *
             * The default implementation of this desctructor is empty.
             */
            virtual ~AbstractValue()
            {
            }
    };
}


#endif
