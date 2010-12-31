#ifndef _INTEGER_H_
#define _INTEGER_H_

#include <istream>

#include "types.h"
#include "variable.h"
#include "exceptions.h"

namespace agentx
{
    /**
     * \brief Represents an Integer as descibed in RFC 2741
     */
    class Integer : public variable
    {
	protected:
	    /**
	     * \brief The Integer value
	     */
	    uint32_t value;

	public:
	    /**
	     * \brief Create an Integer without initialization.
	     *
	     * The value after creation is undefined.
	     */
	    Integer() {}
	    
	    /**
	     * \internal
	     *
	     * \brief Encode the object as described in RFC 2741, section 5.4
	     *
	     * This function uses big endian.
	     */
	    virtual data_t serialize();
	    
	    /**
	     * \internal
	     *
	     * \brief Construct an Integer object from an input stream
	     *
	     * This constructor parses a serialized Integer. The Integer is 
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
	     * 
	     */
	    Integer(std::istream& in, bool big_endian=true) throw(parse_error);
    };
}

#endif
