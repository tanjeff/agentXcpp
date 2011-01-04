#include "OpenPDU.h"

using namespace agentx;

OpenPDU::OpenPDU(data_t::const_iterator& pos, bool big_endian)
    : PDU(pos, big_endian)
{
    // header is parsed by base class constructor
    // so we continue with timeout field:
    timeout = *pos++;
    pos += 3;	// skip reserved fields

    id = new oid(pos, big_endian);

    descr = new Octet_String(pos, big_endian);
}
