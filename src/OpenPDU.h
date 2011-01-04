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
	    oid* id;
	    Octet_String* descr;

	public:
	    /**
	     * \brief Parse constructor
	     *
	     * Construct the object by parsing a stream.
	     *
	     * \param pos Iterator pointing to the current stream position.
	     *            The iterator is advanced while reading the header.
	     *
	     * \param big_endian Wether the serialized form of the PDU is in
	     *                   big_endian format.
	     *
	     * \exception parse_error If parsing fails, for example because
	     *			      reading the stream fails or the PDU is 
	     *			      malformed.
	     */
	    OpenPDU(data_t::const_iterator& pos, bool big_endian);
    };
}


#endif
