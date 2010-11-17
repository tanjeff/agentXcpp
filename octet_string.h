#ifndef _OCTET_STRING_H_
#define _OCTET_STRING_H_

#include "types.h"
#include "variable.h"

namespace agentx
{
    /**
     * \brief Represents an Octet String as descibed in RFC 2741, section 5.3
     */
    class octet_string : public variable
    {
	private:
	    // We use data_t here although it was not invented for this.
	    // TODO: Is this a problem?
	    data_t value;

	public:
	    /**
	     * \brief Encode the object as described in RFC 2741, section 5.3
	     *
	     * Note:
	     * We always use big endian.
	     */
	    data_t serialize();


	    void deserialize(data_t data, bool big_endian=false);

	    virtual uint16_t syntax()
	    {
		return 4;	// RFC 2741, section 5.4
	    }

	    virtual data_t get() = 0;
    };
}

#endif
