#include "octet_string.h"

using namespace agentx;

data_t octet_string::serialize()
{
    data_t serialized;

    // encode size (big endian)
    int size = value.size();
    serialized.push_back(size << 24 & 0xff);
    serialized.push_back(size << 16 & 0xff);
    serialized.push_back(size << 8 & 0xff);
    serialized.push_back(size << 0 & 0xff);

    // encode value
    serialized += value;
}


void octet_string::deserialize(data_t data, bool big_endian)
{
    uint32_t size;
    if( big_endian )
    {
	size =  data[0] << 24;
	size |= data[1] << 16;
	size |= data[2] << 8;
	size |= data[3] << 0;
    }
    else
    {
	size =  data[0] << 0;
	size |= data[1] << 8;
	size |= data[2] << 16;
	size |= data[3] << 24;
    }

    value = data.assign(data, 4, data.size()-4);
}
