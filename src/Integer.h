#ifndef _INTEGER_H_
#define _INTEGER_H_

#include "types.h"
#include "variable.h"

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
	     * \brief Deserialize an Integer object.
	     *
	     * This sets the Integer value to the value found in the serialized 
	     * data. The iterator 'pos' is advanced while parsing. After the 
	     * Integer is deserialized, 'pos' points to the first byte after 
	     * the integer. 
	     */
	    void deserialize(data_t::const_iterator& pos, bool big_endian=false);
    };
}

#endif
