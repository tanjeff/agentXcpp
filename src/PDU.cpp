#include "PDU.h"
#include "OpenPDU.h"
#include "ClosePDU.h"
#include "RegisterPDU.h"
#include "helper.h"

using namespace agentx;

PDU::PDU(data_t::const_iterator& pos, bool big_endian) throw(parse_error)
{
    // skip protocol version (already checked by get_pdu()) and
    // type field (not needed here)
    pos += 2;

    // read flags
    byte_t flags = *pos++;
    instance_registration    = ( flags & (1<<0) ) ? true : false;
    new_index                = ( flags & (1<<1) ) ? true : false;
    any_index                = ( flags & (1<<2) ) ? true : false;

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


PDU* PDU::get_pdu(input_stream& in) throw(parse_error, version_mismatch)
{
    data_t buf;	// serialized form of the PDU
    data_t::const_iterator pos;	// needed for parsing

    // read header
    const int header_size = 20;
    byte_t header[header_size];
    in.read(header, header_size);
    if( !in )
    {
	throw( parse_error() );
    }
    buf.append(header, header_size);

    // check protocol version
    byte_t version = buf[0];
    if( version != 1 )
    {
	throw( version_mismatch() );
    }
    
    // read endianess flag
    byte_t flags = buf[2];
    bool big_endian = ( flags & (1<<4) ) ? true : false;
    
    // read payload length
    uint32_t payload_length;
    pos = buf.begin() + 16;
    payload_length = read32(pos, big_endian);
    if( payload_length%4 != 0 )
    {
	// payload length must be a multiple of 4!
	// See RFC 2741, 6.1. "AgentX PDU Header"
	throw( parse_error() );
    }

    // read payload
    byte_t* payload = new byte_t[payload_length];
    in.read(payload, payload_length);
    if( !in )
    {
	throw( parse_error() );
    }
    buf.append(payload, payload_length);
    delete[] payload;

    // read type
    byte_t type = buf[1];

    // create PDU
    PDU* pdu;
    pos = buf.begin();
    switch(type)
    {
        case agentxOpenPDU:
            pdu = new OpenPDU(pos, big_endian);
            break;
	case agentxClosePDU:
	    pdu = new ClosePDU(pos, big_endian);
	    break;
	case agentxResponsePDU:
	    pdu = new RegisterPDU(pos, big_endian);
	    break;
	default:
	    // type is invalid
	    throw(parse_error());
    }

    // return created PDU
    return pdu;
}

PDU::PDU()
{
    // Set default values, can be set after object creation:
    sessionID = 0;
    transactionID = 0;
    context = 0;    // null pointer means default context
    
    // Generate packetID
    packetID = ++packetID_cnt;

    instance_registration = false;
    new_index = false;
    any_index = false;
}


void PDU::add_header(byte_t type, data_t& payload)
{
    /* Add context to payload, if any */
    if(context)
    {
	// insert serialized context at position 0:
	payload.insert(0, context->serialize());
    }
    
    /* Construct header */
    data_t header;

    // Protocol version
    header.push_back(1);

    // Type
    header.push_back(type);

    // flags
    byte_t flags = 0;
    if(instance_registration) flags |= (1<<0);
    if(new_index)             flags |= (1<<1);
    if(any_index)             flags |= (1<<2);
    if(context)               flags |= (1<<3);  // NON_DEFAULT_INDEX
		              flags |= (1<<4);	// We always use big endian
    header.push_back(flags);

    // reserved field
    header.push_back(0);

    // remaining fields
    write32(header, sessionID);
    write32(header, transactionID);
    write32(header, packetID);
    write32(header, payload.size());	// payload length
    
    // Add header to payload
    payload.insert(0, header);
}
