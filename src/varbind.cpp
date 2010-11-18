#include "varbind.h"
#include "octet_string.h"
#include "integer.h"

using namespace agentx;

// RFC 2741, section 5.4
const uint16_t NONE = 0;	// not in RFC
const uint16_t Integer = 2;
const uint16_t Octet_String = 4;
const uint16_t Null = 5;
const uint16_t Object_Identifier = 6;
const uint16_t IpAddress = 64;
const uint16_t Counter32 = 65;
const uint16_t Gauge32 = 66;
const uint16_t TimeTicks = 67;
const uint16_t Opaque = 68;
const uint16_t Counter64 = 70;
const uint16_t noSuchObject = 128;
const uint16_t noSuchInstance = 129;
const uint16_t endOfMibView = 130;


data_t varbind::serialize()
{
    data_t serialized;

    // encode type
    uint16_t type = NONE;
    if( dynamic_cast<octet_string*>(var) ) type = 4;
    if( dynamic_cast<integer*>(var) ) type = 2;
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


