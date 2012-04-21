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
#ifndef _VARIABLE_H_
#define _VARIABLE_H_

#include "types.hpp"

namespace agentxcpp
{
    /**
     * \brief This class represents a SNMP variable.
     *
     * It is inherited by more concrete classes.
     */
    class variable
    {
	public:
	    /**
             * \internal
             *
	     * \brief Serialize the variable.
	     *
	     * This function must be implemented by all derived classes. The 
	     * function shall generate a serialized form of the variable.
	     *
	     * \return The serialized form of the variable.
	     *
	     * \exception None: The function shall not throw.
	     */
	    // Needed for encoding varbinds
	    virtual data_t serialize() const = 0;

	/**
	 * \brief An SNMP GET request was received.
	 *
	 * This function should be implemented by derived classes to support 
	 * GET requests. The function shall update its internal data if 
	 * neccessary.
	 *
	 * This function is called on SNMP GET requests, before serializing the 
	 * variable and sending it to the master agent.
	 *
	 * \exception None.
	 */
	//virtual void get();

	/**
         * \internal
         *
	 * \brief Update the internal state of the variable.
	 *
	 * This function must be implemented in derived classes. It shall 
	 * update the internal state of the object.
	 *
         * \exception generic_error If obtaining the new value failed. The
         *                          state of such an object cannot be 
         *                          updated.
	 */
	virtual void update()=0;
    };
}


#endif
