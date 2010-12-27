#ifndef _GAUGE32_H_
#define _GAUGE32_H_

#include "types.h"
#include "variable.h"

namespace agentx
{
    /**
     * \brief Represents an Integer as descibed in RFC 2741
     */
    class Gauge32 : public variable
    {
	protected:
	    /**
	     * \brief The value
	     */
	    uint32_t value;

	public:
	    /**
	     * \brief Create a Gauge32 without initialization.
	     *
	     * The value after creation is undefined.
	     */
	    Gauge32() {}
	    
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
	     * \brief Deserialize an Gauge32 object.
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
