#include "OpenPDU.h"

using namespace agentx;

OpenPDU::OpenPDU(data_t::const_iterator& pos, bool big_endian)
    : PDU(pos, big_endian)
{
    // header is parsed by base class constructor
    // so we continue with timeout field:
    timeout = *pos++;
    pos += 3;	// skip reserved fields

    id = oid(pos, big_endian);

    descr = Octet_String(pos, big_endian);
}
	    

OpenPDU::OpenPDU(Octet_String _descr, oid _id, byte_t _timeout)
{
    timeout = _timeout;
    id = _id;
    descr = _descr;
}


data_t OpenPDU::serialize()
{
    data_t serialized;

    // timeout and reserved fields
    serialized.push_back(timeout);
    serialized.push_back(0);
    serialized.push_back(0);
    serialized.push_back(0);

    // id
    serialized += id.serialize();

    // descr
    serialized += descr.serialize();

    // Add header (type for OpenPDU is 1)
    add_header(1, serialized);

    // return serialized form of PDU
    return serialized;
}
