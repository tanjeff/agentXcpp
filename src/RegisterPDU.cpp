#include "RegisterPDU.h"
#include "helper.h"

using namespace agentx;

RegisterPDU::RegisterPDU(
	data_t::const_iterator& pos,
	bool big_endian) throw(parse_error)
    :PDU(pos, big_endian)   // parse header
{
    timeout = read32(pos, big_endian);
    priority = read32(pos, big_endian);
    range_subid = read32(pos, big_endian);
    pos++;  // skip reserved field

    subtree = oid(pos, big_endian);

    // read r.upper_bound only if r.range_subid is not 0
    if( range_subid )
    {
	upper_bound = oid(pos, big_endian);
    }
}


data_t RegisterPDU::serialize()
{
    data_t serialized;

    serialized.push_back(timeout);
    serialized.push_back(priority);
    serialized.push_back(range_subid);
    serialized.push_back(0);	// reserved

    serialized += subtree.serialize();

    if( range_subid )
    {
	serialized += upper_bound.serialize();
    }

    // Add Header
    add_header(3, serialized);

    return serialized;

}
