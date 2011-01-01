#include "varbind.h"
#include "Octet_String.h"
#include "Integer.h"
#include "Counter32.h"
#include "Counter64.h"
#include "Gauge32.h"
#include "TimeTicks.h"
#include "Opaque.h"
#include "IpAddress.h"

using namespace agentx;


data_t varbind::serialize()
{
    data_t serialized;

    // encode type
    serialized.push_back( type << 8 & 0xff );
    serialized.push_back( type << 0 & 0xff );
    
    // reserved field
    serialized.push_back( 0 );	// reserved
    serialized.push_back( 0 );	// reserved
    
    // encode name
    serialized += name->serialize();

    // encode data if needed
    if (var) serialized += var->serialize();

    return serialized;
}


varbind::varbind(oid* o, variable* v) throw(inval_param)
{
    name = o;
    var = v;

    // Determine type of variable and fill type field.
    if( dynamic_cast<Integer*>(var) ) type = 2;
    else if( dynamic_cast<Octet_String*>(var) ) type = 4;
    else if( dynamic_cast<oid*>(var) ) type = 6;
    else if( dynamic_cast<IpAddress*>(var) ) type = 64;
    else if( dynamic_cast<Counter32*>(var) ) type = 65;
    else if( dynamic_cast<Gauge32*>(var) ) type = 66;
    else if( dynamic_cast<TimeTicks*>(var) ) type = 67;
    else if( dynamic_cast<Opaque*>(var) ) type = 68;
    else if( dynamic_cast<Counter64*>(var) ) type = 70;
    else
    {
	// Type could not be determined -> invalid parameter.
	throw inval_param();
    }
}


varbind::varbind(oid* o, type_t t) throw(inval_param)
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

varbind::varbind(input_stream& in, bool big_endian) throw(parse_error)
{
    uint16_t type;
    
    // Get type
    if( big_endian )
    {
	type |= in.get() << 8;
	type |= in.get() << 0;
    }
    else
    {
	type |= in.get() << 0;
	type |= in.get() << 8;
    }

    // skip reserved field
    in.seekg(2, std::ios_base::cur);
    
    // check stream
    if(!in)
    {
	throw(parse_error());
    }

    // TODO: read OID!
    name = new oid(in, big_endian);

    // Get data: no exceptions are catched; they are forwarded to the caller
    switch(type)
    {
	case 2:
	    var = new Integer(in, big_endian);
	    break;
	default:
	    // invalid type
	    throw(parse_error());
    }
}
