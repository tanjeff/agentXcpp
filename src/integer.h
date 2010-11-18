#ifndef _INTEGER_H_
#define _INTEGER_H_

#include "types.h"
#include "variable.h"

namespace agentx
{
    /**
     * \brief Represents an Octet String as descibed in RFC 2741, section 5.3
     */
    class integer : public variable
    {
	private:
	    uint32_t value;

	public:
	    /**
	     * \brief Encode the object as described in RFC 2741, section 5.4
	     *
	     * Note:
	     * We always use big endian.
	     */
	    data_t serialize();

	    integer(uint32_t initial_value) : value(initial_value) {}


	    void deserialize(data_t::const_iterator begin, bool big_endian=false);

	    virtual data_t get() = 0;

	    /**
	     * \brief Set the current value
	     */
	    void set_value(uint32_t new_value) { value = new_value; }

	    /**
	     * \brief get the current value
	     */
	    uint32_t get_value() { return value; }
    };
}

#endif
