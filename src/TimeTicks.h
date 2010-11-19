#ifndef _TIMETICKS_H_
#define _TIMETICKS_H_

#include "types.h"
#include "variable.h"

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
	     * \brief Encode the object as described in RFC 2741, section 5.4
	     *
	     * This function uses big endian.
	     */
	    virtual data_t serialize();
	    
	    /**
	     * \brief Deserialize an TimeTicks object.
	     *
	     * This sets the TimeTicks value to the value found in the 
	     * serialized data.
	     */
	    void deserialize(data_t::const_iterator begin, bool big_endian=false);
    };
}

#endif
