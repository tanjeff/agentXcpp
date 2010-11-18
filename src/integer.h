#ifndef _INTEGER_H_
#define _INTEGER_H_

#include "types.h"
#include "variable.h"

namespace agentx
{
    /**
     * \brief Represents an Integer as descibed in RFC 2741
     */
    class integer : public variable
    {
	protected:
	    /**
	     * \brief The integer value
	     */
	    uint32_t value;

	public:
	    /**
	     * \brief Create an integer without initialization.
	     *
	     * The value after creation is undefined.
	     */
	    integer() {}
	    
	    /**
	     * \brief Encode the object as described in RFC 2741, section 5.4
	     *
	     * This function uses big endian.
	     */
	    virtual data_t serialize();
	    
	    /**
	     * \brief Deserialize an integer object.
	     *
	     * This sets the integer value to the value found in the serialized 
	     * data.
	     */
	    void deserialize(data_t::const_iterator begin, bool big_endian=false);
    };
}

#endif
