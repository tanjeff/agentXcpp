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
#include <boost/cstdint.hpp>

#include "varbind.hpp"
#include "OctetStringValue.hpp"
#include "IntegerValue.hpp"
#include "Counter32Value.hpp"
#include "Counter64Value.hpp"
#include "Gauge32Value.hpp"
#include "TimeTicks.hpp"
#include "OpaqueValue.hpp"
#include "IpAddress.hpp"
#include "util.hpp"

using namespace agentxcpp;
using boost::uint16_t;


binary varbind::serialize() const
{
    binary serialized;

    // encode type
    serialized.push_back( type << 8 & 0xff );
    serialized.push_back( type << 0 & 0xff );
    
    // reserved field
    serialized.push_back( 0 );	// reserved
    serialized.push_back( 0 );	// reserved
    
    // encode name
    serialized += name.serialize();

    // encode data if needed
    if (var.get()) serialized += var->serialize();

    return serialized;
}


varbind::varbind(const OidValue& o, boost::shared_ptr<AbstractValue> v)
{
    name = o;
    var = v;

    // Determine type of variable and fill type field.
    if( dynamic_cast<IntegerValue*>(var.get()) ) type = 2;
    else if( dynamic_cast<OctetStringValue*>(var.get()) ) type = 4;
    else if( dynamic_cast<OidValue*>(var.get()) ) type = 6;
    else if( dynamic_cast<IpAddress*>(var.get()) ) type = 64;
    else if( dynamic_cast<Counter32Value*>(var.get()) ) type = 65;
    else if( dynamic_cast<Gauge32Value*>(var.get()) ) type = 66;
    else if( dynamic_cast<TimeTicks*>(var.get()) ) type = 67;
    else if( dynamic_cast<OpaqueValue*>(var.get()) ) type = 68;
    else if( dynamic_cast<Counter64Value*>(var.get()) ) type = 70;
    else
    {
	// Type could not be determined -> invalid parameter.
	throw inval_param();
    }
}


varbind::varbind(const OidValue& o, type_t t)
{
    name = o;

    // Check t
    switch(t)
    {
	case noSuchObject:
	case noSuchInstance:
	case Null:
	case endOfMibView:
	    // OK, store value
	    type = t;
	    break;
	default:
	    // invalid type: Throw exception
	    throw inval_param();
    }
}

varbind::varbind(binary::const_iterator& pos,
		 const binary::const_iterator& end,
		 bool big_endian)
{
    uint16_t type;
    
    // Type and reserved field
    if(end - pos < 4)
    {
	throw(parse_error());
    }
    
    // Get type
    type = read16(pos, big_endian);

    // skip reserved field
    pos += 2;
    
    // read OID: no exceptions are catched; they are forwarded to the caller
    name = OidValue(pos, end, big_endian); 

    // Get data: no exceptions are catched; they are forwarded to the caller
    switch(type)
    {
	case 2:
	    var.reset(new IntegerValue(pos, end, big_endian));
	    break;
	case 4:
	    var.reset(new OctetStringValue(pos, end, big_endian));
	    break;
	case 6:
	    var.reset(new OidValue(pos, end, big_endian));
	    break;
	case 64:
	    var.reset(new IpAddress(pos, end, big_endian));
	    break;
	case 65:
	    var.reset(new Counter32Value(pos, end, big_endian));
	    break;
	case 66:
	    var.reset(new Gauge32Value(pos, end, big_endian));
	    break;
	case 67:
	    var.reset(new TimeTicks(pos, end, big_endian));
	    break;
	case 68:
	    var.reset(new OpaqueValue(pos, end, big_endian));
	    break;
	case 70:
	    var.reset(new Counter64Value(pos, end, big_endian));
	    break;
	case 5:	    // Null
	case 128:   // noSuchObject
	case 129:   // noSuchInstance
	case 130:   // endOfMibView
	    var.reset();
	    break;
	default:
	    // invalid type
	    throw(parse_error());
    }
}
