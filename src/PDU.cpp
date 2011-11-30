/*
 * Copyright 2011 Tanjeff-Nicolai Moos <tanjeff@cccmz.de>
 *
 * This file is part of the agentXcpp library.
 *
 * AgentXcpp is free software: you can redistribute it and/or modify
 * it under the terms of the AgentXcpp library license, version 1, which 
 * consists of the GNU General Public License and some additional 
 * permissions.
 *
 * AgentXcpp is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * See the AgentXcpp library license in the LICENSE file of this package 
 * for more details.
 */


#include <boost/bind.hpp>


#include "PDU.hpp"
#include "OpenPDU.hpp"
#include "ClosePDU.hpp"
#include "RegisterPDU.hpp"
#include "UnregisterPDU.hpp"
#include "ResponsePDU.hpp"
#include "CommitSetPDU.hpp"
#include "UndoSetPDU.hpp"
#include "TestSetPDU.hpp"
#include "CommitSetPDU.hpp"
#include "CleanupSetPDU.hpp"
#include "GetPDU.hpp"
#include "GetNextPDU.hpp"
#include "GetBulkPDU.hpp"
#include "NotifyPDU.hpp"
#include "helper.hpp"

using namespace agentxcpp;
using boost::shared_ptr;


uint32_t PDU::packetID_cnt = 0;



PDU::PDU()
{
    packetID = ++packetID_cnt;

    sessionID = 0;
    transactionID = 0;

    instance_registration=false;
    new_index=false;
    any_index=false;
    non_default_context = false;

}

PDU::PDU(data_t::const_iterator& pos,
	 const data_t::const_iterator& end,
	 bool big_endian)
{
    if(end - pos < 20)
    {
	throw(parse_error());
    }

    // skip protocol version (already checked by get_pdu()) and
    // type field (not needed here)
    pos += 2;

    // read flags
    byte_t flags = *pos++;
    instance_registration    = ( flags & (1<<0) ) ? true : false;
    new_index                = ( flags & (1<<1) ) ? true : false;
    any_index                = ( flags & (1<<2) ) ? true : false;
    non_default_context      = ( flags & (1<<3) ) ? true : false;

    // skip reserved field
    pos++;

    // read sessionID
    sessionID = read32(pos, big_endian);

    // read transactionID
    transactionID = read32(pos, big_endian);

    // read packetID
    packetID = read32(pos, big_endian);

    // skip payload length (not needed here)
    pos += 4;
}


// Helper type for PDU::get_pdu(). See there for an explanation.
enum status_t {
    in_progress = 0,
    success     = 1,
    fail        = 2
};


// Helper function for PDU::get_pdu(). See there for an explanation.
static void callback_for_get_pdu(const boost::system::error_code& result,
                                 status_t* retval)
{
    if( result.value() == 0 )
    {
	// success
	*retval = success;
    }
    else
    {
	// error
	*retval = fail;
    }
}

/*
 * Timeout handling:
 * 
 * To realize a read opration with a timeout using boost::asio, a callback 
 * function is utilized. First a timer is started, then an asyncronous read 
 * operation is invoked (in fact, two reads are involved: first one for the PDU 
 * header (to know the PDU size), second one for the remaining PDU data). Both, 
 * the timer and the read operation, are set up to invoke the 
 * callback_for_get_pdu() function when they finish. This callback function 
 * writes the result into a variable which was handed over as a pointer, making 
 * the result accessible to the get_pdu() function (different variables are 
 * used for the timer and the read).
 *
 * The variables holding the result are of type status_t and are initialized to 
 * 'in_progress'.
 * 
 * After setting up the timer and the read operation, the io_service.run_one() 
 * function is invoked repeatedly until either the timer or the read finishes.  
 * Note that io_service.run_one() may service other asynchronous operations 
 * first, e.g.  a get request.
 *
 * After that, the results of the timer and the read are inspected. A failure 
 * (e.g. timer expired sucessfully or read failed) leads to an exception.
 */
PDU* PDU::get_pdu(boost::asio::local::stream_protocol::socket& in,
		  unsigned timeout)
{
    // serialized form of the PDU
    data_t buf;

    // needed for parsing
    data_t::const_iterator pos;

    // Start timeout timer
    boost::asio::deadline_timer timer(in.get_io_service());
    status_t timer_result = in_progress;
    timer.expires_from_now( boost::posix_time::milliseconds(timeout) );
    timer.async_wait( boost::bind(callback_for_get_pdu,
				  boost::asio::placeholders::error,
				  &timer_result) );

    // read header
    {
	// Temporary buffer
	const int header_size = 20;
	byte_t header[header_size];

	// Start read
	status_t read_result = in_progress;
	async_read(in,
		   boost::asio::buffer(header, header_size),
		   boost::bind(callback_for_get_pdu,
			       boost::asio::placeholders::error,
			       &read_result));

	// process asio events until read succeeds or timeout expires
	do
	{
	    in.get_io_service().run_one();
	}
	while(read_result == in_progress && timer_result == in_progress);

	// Check result
	if (read_result == success )
	{
	    // Read succeeded: OK

	    // read succeeded: store received data
	    buf.append(header, header_size);

	    // Don't stop timer: the payload has to be read
	    goto header_was_read;
	}
	if (read_result == fail)
	{
	    // read failed: cancel timer, throw exception
	    timer.cancel();
	    throw( network() );
	}
	if (read_result == in_progress && timer_result == success)
	{
	    // timer fired while reading
	    // cancel read, throw exception
	    in.cancel();
	    throw( timeout_exception() );
	}
	if (read_result == in_progress && timer_result == fail)
	{
	    // timer failed while reading
	    // what now?
	    // I think we should fail with a network error
	    in.cancel();
	    throw( network() );
	}
    }

header_was_read:

    // check protocol version
    byte_t version = buf[0];
    if( version != 1 )
    {
	// Wrong protocol:
	// cancel timer and throw exception
	timer.cancel();
	throw( version_mismatch() );
    }

    // read endianess flag
    byte_t flags = buf[2];
    bool big_endian = ( flags & (1<<4) ) ? true : false;

    // read payload length
    uint32_t payload_length;
    pos = buf.begin() + 16;
    payload_length = read32(pos, big_endian);
    if( payload_length % 4 != 0 )
    {
	// payload length must be a multiple of 4!
	// See RFC 2741, 6.1. "AgentX PDU Header"
	// -> cancel timer, throw exception
	timer.cancel();
	throw( parse_error() );
    }

    // read payload
    {
	// Temporary buffer
	byte_t* payload = new byte_t[payload_length];

	// Start read
	status_t read_result = in_progress;
	async_read(in,
		   boost::asio::buffer(payload, payload_length),
		   boost::bind(callback_for_get_pdu,
			       boost::asio::placeholders::error,
			       &read_result));

	// process asio events until read succeeds or timeout expires
	do
	{
	    in.get_io_service().run_one();
	}
	while(read_result == in_progress && timer_result == in_progress);

	// Check result
	if (read_result == success )
	{
	    // Read succeeded:
	    // cancel timer, store received data, delete temporary buffer
	    timer.cancel();
	    buf.append(payload, payload_length);
	    delete[] payload;

	    goto payload_was_read;
	}
	if (read_result == fail)
	{
	    // read failed:
	    // cancel timer, delete temporary buffer, throw exception
	    timer.cancel();
	    delete[] payload;
	    throw( network() );
	}
	if (read_result == in_progress && timer_result == success)
	{
	    // timer fired while reading:
	    // cancel timer, delete temporary buffer, throw exception
	    in.cancel();
	    delete[] payload;
	    throw( timeout_exception() );
	}
	if (read_result == in_progress && timer_result == fail)
	{
	    // timer failed while reading
	    // what now?
	    // I think we should fail with a network error
	    in.cancel();
	    delete[] payload;
	    throw( network() );
	}
    }

payload_was_read:

    // read type
    byte_t type = buf[1];

    // create PDU (TODO: complete the list!)
    PDU* pdu;
    pos = buf.begin();
    const data_t::const_iterator end = buf.end();
    switch(type)
    {
        case agentxOpenPDU:
            pdu = new OpenPDU(pos, end, big_endian);
            break;
	case agentxClosePDU:
	    pdu = new ClosePDU(pos, end, big_endian);
	    break;
	case agentxRegisterPDU:
	    pdu = new RegisterPDU(pos, end, big_endian);
	    break;
	case agentxUnregisterPDU:
	    pdu = new UnregisterPDU(pos, end, big_endian);
	    break;
	case agentxResponsePDU:
	    pdu = new ResponsePDU(pos, end, big_endian);
	    break;
	case agentxCommitSetPDU:
	    pdu = new CommitSetPDU(pos, end, big_endian);
	    break;
	case agentxUndoSetPDU:
	    pdu = new UndoSetPDU(pos, end, big_endian);
	    break;
	case agentxTestSetPDU:
	    pdu = new TestSetPDU(pos, end, big_endian);
	    break;
	case agentxCleanupSetPDU:
	    pdu = new CleanupSetPDU(pos, end, big_endian);
	    break;
	case agentxGetPDU:
	    pdu = new GetPDU(pos, end, big_endian);
	    break;
	case agentxGetNextPDU:
	    pdu = new GetNextPDU(pos, end, big_endian);
	    break;
	case agentxGetBulkPDU:
	    pdu = new GetBulkPDU(pos, end, big_endian);
	    break;
	case agentxNotifyPDU:
	    pdu = new NotifyPDU(pos, end, big_endian);
	    break;
	default:
	    // type is invalid
	    throw(parse_error());
    }

    // return created PDU
    return pdu;
}


shared_ptr<PDU> PDU::parse_pdu(data_t buf)
{
    // needed for parsing
    data_t::const_iterator pos;

    // check protocol version
    byte_t version = buf[0];
    if( version != 1 )
    {
	// Wrong protocol:
	// throw exception
	throw( version_mismatch() );
    }

    // read endianess flag
    byte_t flags = buf[2];
    bool big_endian = ( flags & (1<<4) ) ? true : false;

    // read payload length
    uint32_t payload_length;
    pos = buf.begin() + 16;
    payload_length = read32(pos, big_endian);
    if( payload_length % 4 != 0 )
    {
	// payload length must be a multiple of 4!
	// See RFC 2741, 6.1. "AgentX PDU Header"
	// -> throw exception
	throw( parse_error() );
    }

    // read PDU type
    byte_t type = buf[1];

    // create PDU (TODO: complete the list!)
    shared_ptr<PDU> pdu;
    pos = buf.begin();
    const data_t::const_iterator end = buf.end();
    switch(type)
    {
	case agentxOpenPDU:
	    pdu.reset(new OpenPDU(pos, end, big_endian));
	    break;
	case agentxClosePDU:
	    pdu.reset(new ClosePDU(pos, end, big_endian));
	    break;
	case agentxRegisterPDU:
	    pdu.reset(new RegisterPDU(pos, end, big_endian));
	    break;
	case agentxUnregisterPDU:
	    pdu.reset(new UnregisterPDU(pos, end, big_endian));
	    break;
	case agentxResponsePDU:
	    pdu.reset(new ResponsePDU(pos, end, big_endian));
	    break;
	case agentxCommitSetPDU:
	    pdu.reset(new CommitSetPDU(pos, end, big_endian));
	    break;
	case agentxUndoSetPDU:
	    pdu.reset(new UndoSetPDU(pos, end, big_endian));
	    break;
	case agentxTestSetPDU:
	    pdu.reset(new TestSetPDU(pos, end, big_endian));
	    break;
	case agentxCleanupSetPDU:
	    pdu.reset(new CleanupSetPDU(pos, end, big_endian));
	    break;
	case agentxGetPDU:
	    pdu.reset(new GetPDU(pos, end, big_endian));
	    break;
	case agentxGetNextPDU:
	    pdu.reset(new GetNextPDU(pos, end, big_endian));
	    break;
	case agentxGetBulkPDU:
	    pdu.reset(new GetBulkPDU(pos, end, big_endian));
	    break;
	case agentxNotifyPDU:
	    pdu.reset(new NotifyPDU(pos, end, big_endian));
	    break;
	default:
	    // type is invalid
	    throw(parse_error());
    }

    // return created PDU
    return pdu;
}



void PDU::add_header(type_t type, data_t& payload) const
{
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
    if(non_default_context)   flags |= (1<<3);
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
