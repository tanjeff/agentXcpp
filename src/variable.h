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

#include "types.h"

/**
 * \internal
 *
 * \brief This class represents a SNMP variable.
 *
 * It is inherited by more concrete classes.
 */
class variable
{
    public:
	//virtual data_t serialize();
	
	// Needed for encoding varbinds
	virtual data_t serialize() const = 0;
};


#endif
