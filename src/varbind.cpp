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

