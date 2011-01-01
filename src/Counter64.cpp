#include "Counter64.h"

using namespace agentx;

data_t Counter64::serialize()
{
    data_t serialized;

    // encode value (big endian)
    serialized.push_back(value >> 56 & 0xff);
    serialized.push_back(value >> 48 & 0xff);
    serialized.push_back(value >> 40 & 0xff);
    serialized.push_back(value >> 32 & 0xff);
    serialized.push_back(value >> 24 & 0xff);
    serialized.push_back(value >> 16 & 0xff);
    serialized.push_back(value >> 8 & 0xff);
    serialized.push_back(value >> 0 & 0xff);

    return serialized;
}


Counter64::Counter64(input_stream& in, bool big_endian) throw(parse_error)
{
    uint32_t size;

    // Get value
    if( big_endian )
    {
	value =  static_cast<uint64_t>(in.get()) << 56;
	value |= static_cast<uint64_t>(in.get()) << 48;
	value |= static_cast<uint64_t>(in.get()) << 40;
	value |= static_cast<uint64_t>(in.get()) << 32;
	value |= static_cast<uint64_t>(in.get()) << 24;
	value |= static_cast<uint64_t>(in.get()) << 16;
	value |= static_cast<uint64_t>(in.get()) << 8;
	value |= static_cast<uint64_t>(in.get()) << 0;
    }
    else
    {
	value =  static_cast<uint64_t>(in.get()) << 0;
	value |= static_cast<uint64_t>(in.get()) << 8;
	value |= static_cast<uint64_t>(in.get()) << 16;
	value |= static_cast<uint64_t>(in.get()) << 24;
	value |= static_cast<uint64_t>(in.get()) << 32;
	value |= static_cast<uint64_t>(in.get()) << 40;
	value |= static_cast<uint64_t>(in.get()) << 48;
	value |= static_cast<uint64_t>(in.get()) << 56;
    }
    
    // check stream
    if(!in)
    {
	throw(parse_error());
    }
}
