#include "varbind.h"

using namespace agentx;

data_t varbind::serialize()
{
    data_t serialized;

    // encode type
    uint16_t syntax = data->syntax();
    serialized.push_back( syntax << 8 & 0xff );
    serialized.push_back( syntax << 0 & 0xff );
    serialized.push_back( 0 );	// reserved
    serialized.push_back( 0 );	// reserved
    
    // encode name
    serialized += name->serialize(0);

    // encode data
    serialized += data->serialize();
}


varbind::varbind(oid&)
{
}


varbind::varbind(oid&, data_t&)
{
}
