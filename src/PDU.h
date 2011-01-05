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
	    
	    // header flags
	    bool instance_registration;
	    bool new_index;
	    bool any_index;
	    
	    /**
	     * \brief h.sessionID field
	     *
	     * According to RFC 2741, 6.1. "AgentX PDU Header"
	     */
	    uint32_t sessionID;
	    
	    /**
	     * \brief h.transactionID field
	     *
	     * According to RFC 2741, 6.1. "AgentX PDU Header"
	     */
	    uint32_t transactionID;

	protected:
	    /**
	     * \brief Initialize base class part of a PDU
	     */
	    PDU();
	    
	    /**
	     * \brief h.packetID field
	     *
	     * According to RFC 2741, 6.1. "AgentX PDU Header". Is 
	     * automatically filled by constructors, is set to another value by 
	     * ResponsePDU. The PDU class has no setter for this member.
	     *
	     * The ResponsePDU is a special case in resepect of packetIDs, 
	     * therefore this member is protected to allow the ResponsePDU to 
	     * alter it.
	     */
	    uint32_t packetID;
	    
	    /**
	     * \brief Counter for automatic packetID generator
	     *
	     * The packetID member is set automatically by the constructors; 
	     * each new PDU gets a new packetID. This member contains the last 
	     * used packetID.
	     *
	     * The ResponsePDU is a special case in resepect of packetIDs, 
	     * therefore this member is protected to allow the ResponsePDU to 
	     * alter it.
	     */
	    static uint32_t packetID_cnt;
	    


	public:
	    /**
	     * \brief Get sessionID
	     */
	    uint32_t get_sessionID() { return sessionID; }
	    /**
	     * \brief Set sessionID
	     */
	    void set_sessionID(uint32_t id) { sessionID = id; }

	    /**
	     * \brief Get transactionID
	     */
	    uint32_t get_transactionID() { return transactionID; }
	    /**
	     * \brief Set transactionID
	     */
	    void set_transactionID(uint32_t id) { transactionID = id; }
	    
	    /**
	     * \brief Get packetID
	     */
	    uint32_t get_packetID() { return packetID; }

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
	     * \exception version_mismatch If the AgentX version of the PDU
	     *                             in the stream is not 1.  The stream 
	     *                             position is undefined after this 
	     *                             error.
	     *
	     * \param in the input stream to read from
	     *
	     * \return Pointer to PDU object of according type; the user must
	     *	       delete the object if it is not longer needed.
	     */
	    static PDU* get_pdu(input_stream& in) throw(parse_error, version_mismatch);

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
	     * \exception parse_error If parsing fails, for example because
	     *			      reading the stream fails or the PDU is 
	     *			      malformed.
	     */
	    PDU(data_t::const_iterator& pos, bool big_endian) throw(parse_error);

	    /**
	     * \brief Construct the PDU header and add it to the payload
	     *
	     * Add the PDU header to the payload. This also adds the context, 
	     * although it is not strictly part of the header. The payload must 
	     * not grow or shrink after a call to this function as its size is 
	     * encoded into the header.
	     *
	     * The header is encoded in big endian format.
	     *
	     * \param type The PDU type, according to RFC 2741, 6.1. "AgentX
	     *             PDU Header"
	     *
	     * \param payload The payload of the PDU, excluding the context.
	     *		      The header is added to the payload, i.e. the 
	     *		      payload is altered by this function.
	     */
	    void add_header(byte_t type, data_t& payload);

    };
}


#endif


