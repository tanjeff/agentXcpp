#ifndef __PDU_H__
#define __PDU_H__

#include "types.h"
#include "exceptions.h"
#include "Octet_String.h"

namespace agentx
{
    /**
     * \internal
     *
     * \brief The base class of all PDU's
     *
     * This class is never instantiated itself, it only serves as base class 
     * for the various PSU classes (e.g. OpenPDU, RegisterPDU, ...).
     *
     * This class provides the static class method PDU::get_pdu() which reads a 
     * PDU of arbitrary type from an input stream and constructs an appropriate 
     * PDU object.
     */
    class PDU
    {
	private:
	    /**
	     * \brief The PDU context
	     *
	     * The PDU context, if any. If this field is NULL, the PDU has the 
	     * default context.
	     *
	     * This field is not interpreted. It is parsed from the serialized 
	     * form of the PDU respectively included when the PDU is 
	     * serialized.
	     */
	    Octet_String* context;

	    /**
	     * \brief The PDU types with their values
	     *
	     * The PDU types according to RFC 2741, section 6.1 "AgentX PDU 
	     * Header".
	     */
	    enum type_t
	    {
		agentxOpenPDU             = 1,
		agentxClosePDU            = 2,
		agentxRegisterPDU         = 3,
		agentxUnregisterPDU       = 4,
		agentxGetPDU              = 5,
		agentxGetNextPDU          = 6,
		agentxGetBulkPDU          = 7,
		agentxTestSetPDU          = 8,
		agentxCommitSetPDU        = 9,
		agentxUndoSetPDU          = 10,
		agentxCleanupSetPDU       = 11,
		agentxNotifyPDU           = 12,
		agentxPingPDU             = 13,
		agentxIndexAllocatePDU    = 14,
		agentxIndexDeallocatePDU  = 15,
		agentxAddAgentCapsPDU     = 16,
		agentxRemoveAgentCapsPDU  = 17,
		agentxResponsePDU         = 18

	    };


	public:
	    /**
	     * \brief read the PDU from an input stream
	     *
	     * Read the PDU into a buffer, then create a PDU of the according 
	     * type (e.g. OpenPDU) from that buffer.
	     *
	     * If the PDU version is not 1, version_mismatch is thrown. On 
	     * parse error, parse_error is thrown. In case of error, 
	     *
	     * \exception parse_error If parsing fails, for example because
	     *			      reading the stream fails or the PDU is 
	     *			      malformed.
	     *
	     * \param in the input stream to read from
	     *
	     * \return Pointer to PDU object of according type; the user must
	     *	       delete the object if it is not longer needed.
	     */
	    static PDU* get_pdu(input_stream& in) throw(parse_error);

	    /**
	     * \brief Construct the common part of a PDU object
	     *
	     * Read the PDU header from a buffer and initialize part of the PDU 
	     * object.	     
	     * 
	     * \param pos Iterator pointing to the current stream position. The
	     *		  iterator is advanced while reading the header.
	     *
	     * \param big_endian Wether the serialized form of the PDU is in
	     *                   big_endian format.
	     *
	     * \exception version_mismatch If the AgentX version of the PDU in
	     *                             the stream is not 1. The stream 
	     *                             position is undefined after this 
	     *                             error.
	     * \exception parse_error If parsing fails, for example because
	     *			      reading the stream fails or the PDU is 
	     *			      malformed.
	     */
	    PDU(data_t::const_iterator& pos, bool big_endian) throw(parse_error, version_mismatch);

    };
}


#endif


