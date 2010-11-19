#include "varbind.h"
#include "octet_string.h"
#include "Integer.h"
#include "counter32.h"
#include "Gauge32.h"
#include "TimeTicks.h"

using namespace agentx;


data_t varbind::serialize()
{
    data_t serialized;

    // encode type according to RFC 2741, section 5.4
    uint16_t type = 0;   // not in RFC
    if( dynamic_cast<octet_string*>(var) ) type = 4;
    if( dynamic_cast<Integer*>(var) ) type = 2;
    if( dynamic_cast<counter32*>(var) ) type = 65;
    if( dynamic_cast<Gauge32*>(var) ) type = 66;
    if( dynamic_cast<TimeTicks*>(var) ) type = 67;
    serialized.push_back( type << 8 & 0xff );
    serialized.push_back( type << 0 & 0xff );
    
    // reserved field
    serialized.push_back( 0 );	// reserved
    serialized.push_back( 0 );	// reserved
    
    // encode name
    serialized += name->serialize();

    // encode data
    serialized += var->serialize();

    return serialized;
}


varbind::varbind(oid* o, variable* v)
{
    name = o;
    var = v;
}


