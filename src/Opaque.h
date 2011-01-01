#ifndef _OPAQUE_H_
#define _OPAQUE_H_

#include "types.h"
#include "variable.h"
#include "exceptions.h"

namespace agentx
{
    /**
     * \brief Represents an Opaque obejct as descibed in RFC 2741, section 
     * 5.4
     */
    class Opaque : public variable
    {
	protected:
	    // We use data_t here although it was not invented for this.
	    // TODO: Is this a problem?
	    data_t value;

	public:
	    /**
	     * \internal
	     *
	     * \brief Encode the object as described in RFC 2741, section 5.4 / 
	     * 5.3
	     *
	     * Note:
	     * We always use big endian.
	     */
	    data_t serialize();
	    
	    /**
	     * \internal
	     *
	     * \brief Construct a Opaque object from an input stream
	     *
	     * This constructor parses a serialized Opaque. The Opaque is 
	     * removed from the stream during parsing (i.e.  the stream 
	     * position is forwarded).
	     *
	     * The constructor expects valid data from the stream; if parsing 
	     * fails, parse_error is thrown. In this case, the stream position 
	     * is undefined.
	     *
	     * \param in An input stream
	     * \param big_endian Whether the input stream is in big endian
	     *                   format
	     */
	    Opaque(input_stream& in, bool big_endian=true) throw(parse_error);

	    Opaque(data_t initial_value) : value(initial_value) {}

	    Opaque() { }
    };
}

#endif
