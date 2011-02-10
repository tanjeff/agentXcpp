#ifndef __CLOSEPDU_H__
#define __CLOSEPDU_H__

#include "exceptions.h"
#include "types.h"
#include "PDU.h"

namespace agentx
{
    class ClosePDU : public PDU
    {
	public:
	    
	    enum reason_t
	    {
		reasonOther = 1,
		reasonParseError = 2,
		reasonProtocolError = 3,
		reasonTimeouts = 4,
		reasonShutdown = 5,
		reasonByManager = 6
	    };
	    
	private:
	    /**
	     * \brief The reason why the session is being closed
	     */
	    reason_t reason;

	    /**
	     * \brief Hide default constructor
	     */
	    ClosePDU();

	public:
	    /**
	     * \brief Constructor
	     *
	     * \param reason The reason to session closure. Must be a valid
	     *               reason.
	     *
	     * \exception inval_param If the reason value is invalid.
	     */
	    ClosePDU(uint32_t sessionID,
		     reason_t reason,
		     Octet_String* context=0) throw(inval_param);

	    /**
	     * \brief Get the reason
	     */
	    reason_t get_reason() { return reason; }

	    /**
	     * \brief Parse constructor
	     *
	     * Construct the object by parsing a stream. See \ref parsing for 
	     * details about %PDU parsing.
	     *
	     * \param pos Iterator pointing to the current stream position.
	     *            The iterator is advanced while reading the header.
	     *
	     * \param big_endian Whether the serialized form of the %PDU is
	     *                   in big_endian format.
	     *
	     * \exception parse_error If parsing fails, for example because
	     *			      reading the stream fails or the %PDU is 
	     *			      malformed.
	     */
	    ClosePDU(data_t::const_iterator& pos, bool big_endian) throw(parse_error);


	    /**
	     * \brief Serialize the %PDU
	     */
	    data_t serialize();
    };
}

#endif

