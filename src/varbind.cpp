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
#include "varbind.hpp"
#include "Octet_String.hpp"
#include "Integer.hpp"
#include "Counter32.hpp"
#include "Counter64.hpp"
#include "Gauge32.hpp"
#include "TimeTicks.hpp"
#include "Opaque.hpp"
#include "IpAddress.hpp"
#include "helper.hpp"

using namespace agentxcpp;


data_t varbind::serialize() const
{
    data_t serialized;

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


varbind::varbind(const oid& o, boost::shared_ptr<variable> v)
{
    name = o;
    var = v;

    // Determine type of variable and fill type field.
    if( dynamic_cast<Integer*>(var.get()) ) type = 2;
    else if( dynamic_cast<Octet_String*>(var.get()) ) type = 4;
    else if( dynamic_cast<oid*>(var.get()) ) type = 6;
    else if( dynamic_cast<IpAddress*>(var.get()) ) type = 64;
    else if( dynamic_cast<Counter32*>(var.get()) ) type = 65;
    else if( dynamic_cast<Gauge32*>(var.get()) ) type = 66;
    else if( dynamic_cast<TimeTicks*>(var.get()) ) type = 67;
    else if( dynamic_cast<Opaque*>(var.get()) ) type = 68;
    else if( dynamic_cast<Counter64*>(var.get()) ) type = 70;
    else
    {
	// Type could not be determined -> invalid parameter.
	throw inval_param();
    }
}


varbind::varbind(const oid& o, type_t t)
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

varbind::varbind(data_t::const_iterator& pos,
		 const data_t::const_iterator& end,
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
    name = oid(pos, end, big_endian); 

    // Get data: no exceptions are catched; they are forwarded to the caller
    switch(type)
    {
	case 2:
	    var.reset(new Integer(pos, end, big_endian));
	    break;
	case 4:
	    var.reset(new Octet_String(pos, end, big_endian));
	    break;
	case 6:
	    var.reset(new oid(pos, end, big_endian));
	    break;
	case 64:
	    var.reset(new IpAddress(pos, end, big_endian));
	    break;
	case 65:
	    var.reset(new Counter32(pos, end, big_endian));
	    break;
	case 66:
	    var.reset(new Gauge32(pos, end, big_endian));
	    break;
	case 67:
	    var.reset(new TimeTicks(pos, end, big_endian));
	    break;
	case 68:
	    var.reset(new Opaque(pos, end, big_endian));
	    break;
	case 70:
	    var.reset(new Counter64(pos, end, big_endian));
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
