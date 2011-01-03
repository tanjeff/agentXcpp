#include "OpenPDU.h"

using namespace agentx;

OpenPDU::OpenPDU(data_t::const_iterator& pos, bool big_endian)
    : PDU(pos, big_endian)
{
    
}
