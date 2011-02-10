#include "ClosePDU.h"

using namespace agentx;

ClosePDU::ClosePDU(uint32_t _sessionID,
		   reason_t _reason,
		   Octet_String* context) throw(inval_param)
: PDU(context)

{
    // Set sessionID
    set_sessionID(_sessionID);

    // Check parameter
    if( _reason < 1 || _reason > 6 )
    {
	throw(inval_param());
    }

    // store reason
    reason = _reason;
}


ClosePDU::ClosePDU(data_t::const_iterator& pos, bool big_endian) throw(parse_error)
    : PDU(pos, big_endian)
{
    // header is parsed by base class constructor
    // so we continue with reason field:
    reason = static_cast<reason_t>(*pos++);
    pos += 3;	// skip reserved fields

    // check whether reason is valid
    if( reason < 1 || reason > 6)
    {
	// invalid
	throw(parse_error());
    }
}


data_t ClosePDU::serialize()
{
    data_t serialized;

    // Encode reason and reserved fields
    serialized.push_back(reason);
    serialized.push_back(0);
    serialized.push_back(0);
    serialized.push_back(0);

    // Add Header
    add_header(2, serialized);

    return serialized;
}
