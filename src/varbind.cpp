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

#include "varbind.hpp"
#include "OctetStringVariable.hpp"
#include "IntegerVariable.hpp"
#include "Counter32Variable.hpp"
#include "Counter64Variable.hpp"
#include "Gauge32Variable.hpp"
#include "TimeTicksVariable.hpp"
#include "OpaqueVariable.hpp"
#include "IpAddressVariable.hpp"
#include "util.hpp"

using namespace agentxcpp;

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
    if (var) serialized += var->serialize();

    return serialized;
}


varbind::varbind(const OidVariable& o, QSharedPointer<AbstractVariable> v)
{
    name = o;
    var = v;

    // Determine type of variable and fill type field.
    if( qSharedPointerDynamicCast<IntegerVariable>(var) ) type = 2;
    else if( qSharedPointerDynamicCast<OctetStringVariable>(var) ) type = 4;
    else if( qSharedPointerDynamicCast<OidVariable>(var) ) type = 6;
    else if( qSharedPointerDynamicCast<IpAddressVariable>(var) ) type = 64;
    else if( qSharedPointerDynamicCast<Counter32Variable>(var) ) type = 65;
    else if( qSharedPointerDynamicCast<Gauge32Variable>(var) ) type = 66;
    else if( qSharedPointerDynamicCast<TimeTicksVariable>(var) ) type = 67;
    else if( qSharedPointerDynamicCast<OpaqueVariable>(var) ) type = 68;
    else if( qSharedPointerDynamicCast<Counter64Variable>(var) ) type = 70;
    else
    {
	// Type could not be determined -> invalid parameter.
	throw inval_param();
    }
}


varbind::varbind(const OidVariable& o, type_t t)
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
    quint16 type;
    
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
    name = OidVariable(pos, end, big_endian);

    // Get data: no exceptions are catched; they are forwarded to the caller
    switch(type)
    {
	case 2:
	    var = QSharedPointer<AbstractVariable>(new IntegerVariable(pos, end, big_endian));
	    break;
	case 4:
	    var = QSharedPointer<AbstractVariable>(new OctetStringVariable(pos, end, big_endian));
	    break;
	case 6:
	    var = QSharedPointer<AbstractVariable>(new OidVariable(pos, end, big_endian));
	    break;
	case 64:
	    var = QSharedPointer<AbstractVariable>(new IpAddressVariable(pos, end, big_endian));
	    break;
	case 65:
	    var = QSharedPointer<AbstractVariable>(new Counter32Variable(pos, end, big_endian));
	    break;
	case 66:
	    var = QSharedPointer<AbstractVariable>(new Gauge32Variable(pos, end, big_endian));
	    break;
	case 67:
	    var = QSharedPointer<AbstractVariable>(new TimeTicksVariable(pos, end, big_endian));
	    break;
	case 68:
	    var = QSharedPointer<AbstractVariable>(new OpaqueVariable(pos, end, big_endian));
	    break;
	case 70:
	    var = QSharedPointer<AbstractVariable>(new Counter64Variable(pos, end, big_endian));
	    break;
	case 5:	    // Null
	case 128:   // noSuchObject
	case 129:   // noSuchInstance
	case 130:   // endOfMibView
	    var.clear();
	    break;
	default:
	    // invalid type
	    throw(parse_error());
    }
}
