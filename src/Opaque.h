#ifndef _OPAQUE_H_
#define _OPAQUE_H_

#include "types.h"
#include "variable.h"

namespace agentx
{
    /**
     * \brief Represents an Opaque obejct as descibed in RFC 2741, section 
     * 5.4
     */
    class Opaque : public variable
    {
	protected:
	    // We use data_t here although it was not invented for this.
	    // TODO: Is this a problem?
	    data_t value;

	public:
	    /**
	     * \brief Encode the object as described in RFC 2741, section 5.4 / 
	     * 5.3
	     *
	     * Note:
	     * We always use big endian.
	     */
	    data_t serialize();

	    Opaque(data_t initial_value) : value(initial_value) {}

	    Opaque() { }


	    /**
	     * \brief Deserialize an Opaque object.
	     *
	     * This sets the Opaque data to the value found in the serialized 
	     * data.  The iterator 'pos' is advanced while parsing.  After the 
	     * object is deserialized, 'pos' points to the first byte after the 
	     * object.
	     */
	    void deserialize(data_t::const_iterator& pos, bool big_endian=false);


    };
}

#endif
