#include "RegisterPDU.h"
	    
RegisterPDU::RegisterPDU(
	data_t::const_iterator& pos,
	bool big_endian) throw(parse_error)
    :PDU(pos, big_endian)   // parse header
{
    timeout = read32(pos, big_endian);
    priority = read32(pos, big_endian);
    range_subid = read32(pos, big_endian);
    pos++;  // skip reserved field

    // read r.upper_bound only if r.range_subid is not 0
    if( range_subid )
    {
	upper_bound = oid(pos, big_endian);
    }

}
