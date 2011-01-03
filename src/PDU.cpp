#include "PDU.h"
#include "OpenPDU.h"
#include "helper.h"

using namespace agentx;

PDU::PDU(data_t::const_iterator& pos, bool big_endian) throw(parse_error, version_mismatch)
{
    uint32_t sessionID;
    uint32_t transactionID;
    uint32_t packetID;
    
    // check protocol version
    byte_t version = *pos++;
    if( version != 1 )
    {
	throw( version_mismatch() );
    }

    // skip type field (not needed here)
    pos++;

    // read flags
    byte_t flags = *pos++;
    bool INSTANCE_REGISTRATION    = ( flags & (1<<0) ) ? true : false;
    bool NEW_INDEX                = ( flags & (1<<1) ) ? true : false;
    bool ANY_INDEX                = ( flags & (1<<2) ) ? true : false;

    // sessionID
    sessionID = read32(pos, big_endian);

    // transactionID
    transactionID = read32(pos, big_endian);

    // packetID
    packetID = read32(pos, big_endian);

    // skip payload length (not needed here)
    pos += 4;

    // Read context if present
    if( flags & (1<<3) ) // NON_DEFAULT_CONTEXT set?
    {
        context = new Octet_String(pos, big_endian);
    }
    else
    {
        context = 0;	// no context in PDU
    }
}


PDU* PDU::get_pdu(input_stream& in) throw(parse_error)
{
    data_t msg;	// serialized form of the PDU
    data_t::const_iterator pos;	// needed for parsing

    // read header
    const int header_size = 20;
    byte_t header[header_size];
    in.read(header, header_size);
    if( !in )
    {
	throw( parse_error() );
    }
    msg.assign(header, header_size);

    // read endianess flag
    byte_t flags = msg[2];
    bool big_endian = ( flags & (1<<4) ) ? true : false;
    
    // read payload length
    uint32_t payload_length;
    pos = msg.begin() + 16;
    payload_length = read32(pos, big_endian);

    // read payload
    byte_t* payload = new byte_t[payload_length];
    in.read(payload, payload_length);
    if( !in )
    {
	throw( parse_error() );
    }
    msg.append(payload, payload_length);
    delete[] payload;

    // read type
    byte_t type = msg[1];

    // create PDU
    PDU* pdu;
    pos = msg.begin();
    switch(type)
    {
        case agentxOpenPDU:
            pdu = new OpenPDU(pos, big_endian);
            break;
	default:
	    // type is invalid
	    throw(parse_error());
    }
}
