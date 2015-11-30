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

#include "RemoveAgentCapsPDU.hpp"
#include "OidVariable.hpp"


using namespace agentxcpp;

RemoveAgentCapsPDU::RemoveAgentCapsPDU(binary::const_iterator& pos,
				       const binary::const_iterator& end,
				       bool big_endian)
    : PDUwithContext(pos, end, big_endian)
{
    // parse ID
    id = OidVariable(pos, end, big_endian).value();
}
	    



binary RemoveAgentCapsPDU::serialize()
{
    binary serialized;

    // Serialize data
    serialized += OidVariable(id).serialize();

    // Add header
    add_header(PDU::agentxRemoveAgentCapsPDU, serialized);

    // return serialized form of PDU
    return serialized;
}


RemoveAgentCapsPDU::RemoveAgentCapsPDU(Oid id)
{
    this->id = id;
}
