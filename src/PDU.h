#ifndef __PDU_H__
#define __PDU_H__

#include "types.h"
#include "exceptions.h"
#include "Octet_String.h"

namespace agentx
{
    /**
     * \brief The base class of all PDU's
     *
     */
    class PDU
    {
	private:
	    /**
	     * \brief the context field
	     *
	     * When parsing the serialized form of a PDU, this field is filled 
	     * with the context string of the PDU. If no context string was 
	     * transmitted, context is a null pointer.
	     *
	     * When sending a PDU, this field is included in the transmission, 
	     * except if it is a null pointer.
	     */
	    //Octet_String* context;

	    //uint32_t sessionID;
	    //uint32_t transactionID;
	    //uint32_t packetID;
	    
	protected:
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
	    Octet_String* context;

	    /**
	     * \internal
	     *
	     * \brief read a PDU from an input stream
	     *
	     * This reads the PDU header from the input stream and then 
	     * constructs a PDU object of the correct type.
	     *
	     * On parse error, parse_error is thrown.
	     *
	     * \param in the input stream to read from
	     */
	    static PDU* get_pdu(input_stream& in) throw(parse_error, version_mismatch);


    };
}


#endif


