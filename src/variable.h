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
	virtual data_t serialize() = 0;
};


#endif
