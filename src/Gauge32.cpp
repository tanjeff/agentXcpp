#include "Gauge32.h"

using namespace agentx;

data_t Gauge32::serialize()
{
    data_t serialized;

    // encode value (big endian)
    serialized.push_back(value >> 24 & 0xff);
    serialized.push_back(value >> 16 & 0xff);
    serialized.push_back(value >> 8 & 0xff);
    serialized.push_back(value >> 0 & 0xff);

    return serialized;
}


Gauge32::Gauge32(data_t::const_iterator& pos, bool big_endian) throw(parse_error)
{
    uint32_t size;

    // Get value
    if( big_endian )
    {
	value =  *pos++ << 24;
	value |= *pos++ << 16;
	value |= *pos++ << 8;
	value |= *pos++ << 0;
    }
    else
    {
	value =  *pos++ << 0;
	value |= *pos++ << 8;
	value |= *pos++ << 16;
	value |= *pos++ << 24;
    }
}
