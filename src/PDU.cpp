#include "PDU.h"
#include "OpenPDU.h"

using namespace agentx;

PDU* PDU::get_pdu(input_stream& in) throw(parse_error, version_mismatch)
{

    uint32_t sessionID;
    uint32_t transactionID;
    uint32_t packetID;
    
    // check protocol version
    byte_t version = in.get();
    if( !in )
    {
	throw( parse_error() );
    }
    if( version != 1 )
    {
	throw( version_mismatch() );
    }

    // read type and flags
    byte_t type = in.get();
    byte_t flags = in.get();
    bool INSTANCE_REGISTRATION    = ( flags & (1<<0) ) ? true : false;
    bool NEW_INDEX                = ( flags & (1<<1) ) ? true : false;
    bool ANY_INDEX                = ( flags & (1<<2) ) ? true : false;
    bool non_default_context = ( flags & (1<<3) ) ? true : false;
    bool big_endian          = ( flags & (1<<4) ) ? true : false;

    // session_id
    if( big_endian )
    {
	sessionID =  in.get() << 24;
	sessionID |= in.get() << 16;
	sessionID |= in.get() << 8;
	sessionID |= in.get() << 0;
    }
    else
    {
	sessionID =  in.get() << 0;
	sessionID |= in.get() << 8;
	sessionID |= in.get() << 16;
	sessionID |= in.get() << 24;
    }

    // transactionID
    if( big_endian )
    {
	transactionID =  in.get() << 24;
	transactionID |= in.get() << 16;
	transactionID |= in.get() << 8;
	transactionID |= in.get() << 0;
    }
    else
    {
	transactionID =  in.get() << 0;
	transactionID |= in.get() << 8;
	transactionID |= in.get() << 16;
	transactionID |= in.get() << 24;
    }

    // packetID
    if( big_endian )
    {
	packetID =  in.get() << 24;
	packetID |= in.get() << 16;
	packetID |= in.get() << 8;
	packetID |= in.get() << 0;
    }
    else
    {
	packetID =  in.get() << 0;
	packetID |= in.get() << 8;
	packetID |= in.get() << 16;
	packetID |= in.get() << 24;
    }
    
    // payload length
    uint32_t size;
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

    // Read payload
    byte_t* buf = new byte_t[size];
    in.read(buf, size);
    data_t payload;
    payload.assign(buf, size);
    delete[] buf;
    if( !in )
    {
	// stream failure during read
	throw( parse_error() );
    }

    // construct iterator for further pasing
    data_t::const_iterator pos = payload.begin();
    
    // Read context if present
    Octet_String* context;
    if(non_default_context)
    {
        context = new Octet_String(pos, big_endian);
    }
    else
    {
        context = 0;	// no context in PDU
    }
    
    // create PDU
    PDU* pdu;
    switch(type)
    {
        case agentxOpenPDU:
            pdu = new OpenPDU(pos, big_endian);
            break;
    }
    pdu->context = context;
    

}
