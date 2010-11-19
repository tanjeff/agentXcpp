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


void Counter64::deserialize(data_t::const_iterator it, bool big_endian)
{
    uint32_t size;

    // Get value
    if( big_endian )
    {
	value =  static_cast<uint64_t>(*it++) << 56;
	value |= static_cast<uint64_t>(*it++) << 48;
	value |= static_cast<uint64_t>(*it++) << 40;
	value |= static_cast<uint64_t>(*it++) << 32;
	value |= static_cast<uint64_t>(*it++) << 24;
	value |= static_cast<uint64_t>(*it++) << 16;
	value |= static_cast<uint64_t>(*it++) << 8;
	value |= static_cast<uint64_t>(*it++) << 0;
    }
    else
    {
	value =  static_cast<uint64_t>(*it++) << 0;
	value |= static_cast<uint64_t>(*it++) << 8;
	value |= static_cast<uint64_t>(*it++) << 16;
	value |= static_cast<uint64_t>(*it++) << 24;
	value |= static_cast<uint64_t>(*it++) << 32;
	value |= static_cast<uint64_t>(*it++) << 40;
	value |= static_cast<uint64_t>(*it++) << 48;
	value |= static_cast<uint64_t>(*it++) << 56;
    }
}
