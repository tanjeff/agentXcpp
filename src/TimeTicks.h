#ifndef _TIMETICKS_H_
#define _TIMETICKS_H_

#include "types.h"
#include "variable.h"
#include "exceptions.h"

namespace agentx
{
    /**
     * \brief Represents an TimeTicks as descibed in RFC 2741
     */
    class TimeTicks : public variable
    {
	protected:
	    /**
	     * \brief The TimeTicks value
	     */
	    uint32_t value;

	public:
	    /**
	     * \brief Create an TimeTicks without initialization.
	     *
	     * The value after creation is undefined.
	     */
	    TimeTicks() {}
	    
	    /**
	     * \internal
	     *
	     * \brief Construct a TimeTicks object from an input stream
	     *
	     * This constructor parses a serialized TimeTicks. The TimeTicks is 
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
	    TimeTicks(input_stream& in, bool big_endian=true) throw(parse_error);
	    
	    /**
	     * \internal
	     *
	     * \brief Encode the object as described in RFC 2741, section 5.4
	     *
	     * This function uses big endian.
	     */
	    virtual data_t serialize();
    };
}

#endif
