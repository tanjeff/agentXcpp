#ifndef _OCTET_STRING_H_
#define _OCTET_STRING_H_

#include "types.h"
#include "variable.h"
#include "exceptions.h"

namespace agentx
{
    /**
     * \brief Represents an Octet String as descibed in RFC 2741, section 5.3
     */
    class Octet_String : public variable
    {
	protected:
	    // We use data_t here although it was not invented for this.
	    // TODO: Is this a problem?
	    std::basic_string<byte_t> value;

	public:
	    /**
	     * \internal
	     *
	     * \brief Encode the object as described in RFC 2741, section 5.3
	     *
	     * Note:
	     * We always use big endian.
	     */
	    data_t serialize();

	    /**
	     * \brief FIXME
	     */
	    Octet_String(data_t initial_value) : value(initial_value) {}

	    /**
	     * \brief FIXME
	     */
	    Octet_String() { }
	    
	    /**
	     * \internal
	     *
	     * \brief Construct a Octet_String object from an input stream
	     *
	     * This constructor parses a serialized Octet_String. The Octet_String is 
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
	    Octet_String(input_stream& in, bool big_endian=true) throw(parse_error);

	    /**
	     * \brief Set the current value
	     */
	    void set_value(data_t new_value) { value = new_value; }

	    /**
	     * \brief get the current value
	     */
	    data_t get_value() { return value; }
    };
}

#endif
