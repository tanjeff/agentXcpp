#ifndef _INTEGER_H_
#define _INTEGER_H_

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
	     * \brief Construct the object from input stream
	     *
	     * This constructor parses the serialized form of the object.
	     * It takes an iterator, starts parsing at the position of the 
	     * iterator and advances the iterator to the position right behind 
	     * the object.
	     * 
	     * The constructor expects valid data from the stream; if parsing 
	     * fails, parse_error is thrown. In this case, the iterator 
	     * position is undefined.
	     *
	     * \param pos iterator pointing into the stream
	     * \param big_endian Whether the input stream is in big endian
	     *                   format
	     */
	    Integer(data_t::const_iterator& pos, bool big_endian=true) throw(parse_error);
    };
}

#endif
