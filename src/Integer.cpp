#include "Integer.h"

using namespace agentx;

data_t Integer::serialize()
{
    data_t serialized;

    // encode value (big endian)
    serialized.push_back(value >> 24 & 0xff);
    serialized.push_back(value >> 16 & 0xff);
    serialized.push_back(value >> 8 & 0xff);
    serialized.push_back(value >> 0 & 0xff);

    return serialized;
}


Integer::Integer(input_stream& in, bool big_endian) throw(parse_error)
{
    uint32_t size;

    // Get value
    if( big_endian )
    {
	value =  in.get() << 24;
	value |= in.get() << 16;
	value |= in.get() << 8;
	value |= in.get() << 0;
    }
    else
    {
	value =  in.get() << 0;
	value |= in.get() << 8;
	value |= in.get() << 16;
	value |= in.get() << 24;
    }

    // Check for errors
    if(!in)
    {
	throw(parse_error());
    }
}
