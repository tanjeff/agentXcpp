#include "octet_string.h"

using namespace agentx;

data_t octet_string::serialize()
{
    data_t serialized;

    // encode size (big endian)
    int size = value.size();
    serialized.push_back(size >> 24 & 0xff);
    serialized.push_back(size >> 16 & 0xff);
    serialized.push_back(size >> 8 & 0xff);
    serialized.push_back(size >> 0 & 0xff);

    // encode value
    serialized += value;

    // Padding bytes
    int padsize = 4 - (size % 4);
    while( padsize-- )
    {
	serialized.push_back(0);
    }

    return serialized;
}


void octet_string::deserialize(data_t::const_iterator it, bool big_endian)
{
    uint32_t size;

    // Get size
    if( big_endian )
    {
	size =  *it++ << 24;
	size |= *it++ << 16;
	size |= *it++ << 8;
	size |= *it++ << 0;
    }
    else
    {
	size =  *it++ << 0;
	size |= *it++ << 8;
	size |= *it++ << 16;
	size |= *it++ << 24;
    }
    if( size == 0 )
    {
	// nothing left to parse
	return;
    }

    // Get value
    value.assign(it, it+size);
}
