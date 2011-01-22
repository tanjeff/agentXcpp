#ifndef __OPENPDU_H__
#define __OPENPDU_H__

#include "PDU.h"
#include "oid.h"
#include "Octet_String.h"

namespace agentx
{
    /**
     * \internal
     *
     * \brief Represents an Open-PDU
     */
    class OpenPDU : public PDU
    {
	private:
	    byte_t timeout;
	    oid id;
	    Octet_String descr;

	    /**
	     * \brief Hide default constructor
	     */
	    OpenPDU();

	public:
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
	    OpenPDU(data_t::const_iterator& pos, bool big_endian);

	    /**
	     * \brief Constructor
	     *
	     * Sets timeout to 0 (means "no timeout for session specified)
	     *
	     * \param description A string describing the subagent.
	     *
	     * \param ID An Object Identifier that identifies the subagent.
	     *
	     * \param timeout The length of time, in seconds, that a master
	     *		      agent should allow to elapse after dispatching a 
	     *		      message on a session before it regards the 
	     *		      subagent as not responding. This is the default 
	     *		      value for the session, and may be overridden by 
	     *		      values associated with specific registered MIB 
	     *		      regions.  The default value of 0 indicates that 
	     *		      there is no session-wide default value.
	     */
	    OpenPDU(Octet_String description = Octet_String(),
		    oid ID = oid(),
		    byte_t timeout = 0);

	    /**
	     * \brief Serialize the %PDU
	     */
	    data_t serialize();
    };
}


#endif
