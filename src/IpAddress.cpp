#include "IpAddress.h"

using namespace agentx;

data_t IpAddress::serialize()
{
    data_t serialized;

    // encode size (big endian) (size is always 4)
    serialized.push_back(0);
    serialized.push_back(0);
    serialized.push_back(0);
    serialized.push_back(4);

    // encode address
    serialized.push_back(address[0] >> 24 & 0xff);
    serialized.push_back(address[0] >> 16 & 0xff);
    serialized.push_back(address[0] >> 8 & 0xff);
    serialized.push_back(address[0] >> 0 & 0xff);
    serialized.push_back(address[1] >> 24 & 0xff);
    serialized.push_back(address[1] >> 16 & 0xff);
    serialized.push_back(address[1] >> 8 & 0xff);
    serialized.push_back(address[1] >> 0 & 0xff);
    serialized.push_back(address[2] >> 24 & 0xff);
    serialized.push_back(address[2] >> 16 & 0xff);
    serialized.push_back(address[2] >> 8 & 0xff);
    serialized.push_back(address[2] >> 0 & 0xff);
    serialized.push_back(address[3] >> 24 & 0xff);
    serialized.push_back(address[3] >> 16 & 0xff);
    serialized.push_back(address[3] >> 8 & 0xff);
    serialized.push_back(address[3] >> 0 & 0xff);

    return serialized;
}


IpAddress::IpAddress(input_stream& in, bool big_endian) throw(parse_error)
{
    uint32_t size;

    // Get size
    if( big_endian )
    {
	size =  in.get() << 24;
	size |= in.get() << 16;
	size |= in.get() << 8;
	size |= in.get() << 0;
    }
    else
    {
	size =  in.get() << 0;
	size |= in.get() << 8;
	size |= in.get() << 16;
	size |= in.get() << 24;
    }
    if( size != 4 )
    {
	// Parse error; this seems not to be an IpAddress
	throw parse_error();
    }

    // Get address
    if( big_endian )
    {
	address[0] =  in.get() << 24;
	address[0] |= in.get() << 16;
	address[0] |= in.get() << 8;
	address[0] |= in.get() << 0;
	address[1] =  in.get() << 24;
	address[1] |= in.get() << 16;
	address[1] |= in.get() << 8;
	address[1] |= in.get() << 0;
	address[2] =  in.get() << 24;
	address[2] |= in.get() << 16;
	address[2] |= in.get() << 8;
	address[2] |= in.get() << 0;
	address[3] =  in.get() << 24;
	address[3] |= in.get() << 16;
	address[3] |= in.get() << 8;
	address[3] |= in.get() << 0;
    }
    else
    {
	address[0] =  in.get() << 0;
	address[0] |= in.get() << 8;
	address[0] |= in.get() << 16;
	address[0] |= in.get() << 24;
	address[1] =  in.get() << 0;
	address[1] |= in.get() << 8;
	address[1] |= in.get() << 16;
	address[1] |= in.get() << 24;
	address[2] =  in.get() << 0;
	address[2] |= in.get() << 8;
	address[2] |= in.get() << 16;
	address[2] |= in.get() << 24;
	address[3] =  in.get() << 0;
	address[3] |= in.get() << 8;
	address[3] |= in.get() << 16;
	address[3] |= in.get() << 24;
    }
    
    // check stream
    if(!in)
    {
	throw(parse_error());
    }
}
