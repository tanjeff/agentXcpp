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
    while( padsize-- )
    {
	serialized.push_back(0);
    }

    return serialized;
}


Octet_String::Octet_String(input_stream& in, bool big_endian) throw(parse_error)
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
    
    // check stream
    if(!in)
    {
	throw(parse_error());
    }

    // Octet String emtpy?
    if( size == 0 )
    {
	// nothing left to parse
	return;
    }

    // Get value
    byte_t* buf = new byte_t[size];
    in.read(buf, size);	// read 'size' bytes into 'buf'
    value.assign(buf);
}
