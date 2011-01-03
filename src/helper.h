#ifndef __HELPER_H__
#define __HELPER_H__

#include "types.h"

inline uint32_t read32(data_t::const_iterator& pos, bool big_endian)
{
    uint32_t value;
    if( big_endian )
    {
	value =  *pos++ << 24;
	value |= *pos++ << 16;
	value |= *pos++ << 8;
	value |= *pos++ << 0;
    }
    else
    {
	value =  *pos++ << 0;
	value |= *pos++ << 8;
	value |= *pos++ << 16;
	value |= *pos++ << 24;
    }

    return value;
}

inline uint16_t read16(data_t::const_iterator& pos, bool big_endian)
{
    uint16_t value;
    if( big_endian )
    {
	value |= *pos++ << 8;
	value |= *pos++ << 0;
    }
    else
    {
	value =  *pos++ << 0;
	value |= *pos++ << 8;
    }

    return value;
}

#endif
