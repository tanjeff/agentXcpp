#include "integer.h"

using namespace agentx;

data_t integer::serialize()
{
    data_t serialized;

    // encode value (big endian)
    serialized.push_back(value >> 24 & 0xff);
    serialized.push_back(value >> 16 & 0xff);
    serialized.push_back(value >> 8 & 0xff);
    serialized.push_back(value >> 0 & 0xff);

    return serialized;
}


void integer::deserialize(data_t::const_iterator it, bool big_endian)
{
    uint32_t size;

    // Get value
    if( big_endian )
    {
	value =  *it++ << 24;
	value |= *it++ << 16;
	value |= *it++ << 8;
	value |= *it++ << 0;
    }
    else
    {
	value =  *it++ << 0;
	value |= *it++ << 8;
	value |= *it++ << 16;
	value |= *it++ << 24;
    }
}

#include <iostream>

std::ostream& agentx::operator<<(std::ostream& out, const integer& i)
{
    out << i.value;

    return out;
}
