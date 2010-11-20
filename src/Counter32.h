#ifndef _COUNTER32_H_
#define _COUNTER32_H_

#include "types.h"
#include "variable.h"

namespace agentx
{
    /**
     * \brief Represents an Integer as descibed in RFC 2741
     */
    class Counter32 : public variable
    {
	protected:
	    /**
	     * \brief The counter value
	     */
	    uint32_t value;

	public:
	    /**
	     * \brief Create a counter without initialization.
	     *
	     * The value after creation is undefined.
	     */
	    Counter32() {}
	    
	    /**
	     * \brief Encode the object as described in RFC 2741, section 5.4
	     *
	     * This function uses big endian.
	     */
	    virtual data_t serialize();
	    
	    /**
	     * \brief Deserialize an integer object.
	     *
	     * This sets the counter value to the value found in the serialized 
	     * data. The iterator 'pos' is advanced while parsing. After the 
	     * object is deserialized, 'pos' points to the first byte after the 
	     * object.
	     */
	    void deserialize(data_t::const_iterator& pos, bool big_endian=false);
    };
}

#endif
