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


void Counter64::deserialize(data_t::const_iterator& pos, bool big_endian)
{
    uint32_t size;

    // Get value
    if( big_endian )
    {
	value =  static_cast<uint64_t>(*pos++) << 56;
	value |= static_cast<uint64_t>(*pos++) << 48;
	value |= static_cast<uint64_t>(*pos++) << 40;
	value |= static_cast<uint64_t>(*pos++) << 32;
	value |= static_cast<uint64_t>(*pos++) << 24;
	value |= static_cast<uint64_t>(*pos++) << 16;
	value |= static_cast<uint64_t>(*pos++) << 8;
	value |= static_cast<uint64_t>(*pos++) << 0;
    }
    else
    {
	value =  static_cast<uint64_t>(*pos++) << 0;
	value |= static_cast<uint64_t>(*pos++) << 8;
	value |= static_cast<uint64_t>(*pos++) << 16;
	value |= static_cast<uint64_t>(*pos++) << 24;
	value |= static_cast<uint64_t>(*pos++) << 32;
	value |= static_cast<uint64_t>(*pos++) << 40;
	value |= static_cast<uint64_t>(*pos++) << 48;
	value |= static_cast<uint64_t>(*pos++) << 56;
    }
}
