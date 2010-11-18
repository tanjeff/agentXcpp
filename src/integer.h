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
	private:
	    /** \brief the value */
	    uint32_t value;

	public:
	    /**
	     * \brief Encode the object as described in RFC 2741, section 5.4
	     *
	     * Note:
	     * We always use big endian.
	     */
	    virtual data_t serialize();

	    /**
	     * \brief Initialize an Integer object with a value
	     */
	    integer(uint32_t initial_value) : value(initial_value) {}

	    /**
	     * \brief Create an integer without initialization.
	     *
	     * The value after creation is undefined.
	     */
	    integer() {}

	    /**
	     * \brief Deserialize an integer object.
	     *
	     * This sets the integer value to the value found in the serialized 
	     * data.
	     */
	    void deserialize(data_t::const_iterator begin, bool big_endian=false);

	    friend std::ostream& operator<<(std::ostream&, const agentx::integer&);
    };
    
    /**
     * \brief The output operator for the integer class.
     *
     * The integer class uses a builtin integer type to represent its value, 
     * therefore it behaves like int or long would do.
     */
    std::ostream& operator<<(std::ostream&, const agentx::integer&);

}

#endif
