#include <iterator>

#include "Octet_String.h"

using namespace agentx;

data_t Octet_String::serialize()
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
    if( padsize == 4 ) padsize = 0; // avoid adding 4 padding bytes
    while( padsize-- )
    {
	serialized.push_back(0);
    }

    return serialized;
}


Octet_String::Octet_String(data_t::const_iterator& pos, bool big_endian) throw(parse_error)
{
    uint32_t size;

    // Get size
    if( big_endian )
    {
	size =  *pos++ << 24;
	size |= *pos++ << 16;
	size |= *pos++ << 8;
	size |= *pos++ << 0;
    }
    else
    {
	size =  *pos++ << 0;
	size |= *pos++ << 8;
	size |= *pos++ << 16;
	size |= *pos++ << 24;
    }
    
    // Octet String emtpy?
    if( size == 0 )
    {
	// nothing left to parse
	return;
    }

    // Get value
    value.assign(pos, pos+size);
    pos += size;
}
