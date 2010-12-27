#ifndef _IPADDRESS_H_
#define _IPADDRESS_H_

#include "types.h"
#include "variable.h"
#include "exceptions.h"

namespace agentx
{
    /**
     * \brief Represents an IP address as descibed in RFC 2741, section 5.4
     */
    class IpAddress : public variable
    {
	private:
	    /**
	     * \brief Hide default constructor
	     */
	    IpAddress();

	protected:
	    uint32_t address[4]; // only IPv4

	public:
	    /**
	     * \internal
	     *
	     * \brief Encode the object as described in RFC 2741, section 5.4 / 
	     * 5.3
	     *
	     * Note:
	     * We always use big endian.
	     */
	    data_t serialize();

	    /**
	     * \brief FIXME
	     */
	    IpAddress(uint32_t a,
		    uint32_t b,
		    uint32_t c,
		    uint32_t d)
	    {
		address[0] = a;
		address[1] = b;
		address[2] = c;
		address[3] = d;
	    }



	    /**
	     * \internal
	     *
	     * \brief Deserialize an IpAddress object.
	     *
	     * This sets the address of this object to the value found in the 
	     * serialized data.  The iterator 'pos' is advanced while parsing.  
	     * After the object is deserialized, 'pos' points to the first byte 
	     * after the object.
	     *
	     * IpAddress's are encoded as Octet Strings with a size of 16 (4 x 
	     * 32bit). If the size read during deserializing is not 16, then a 
	     * parse_error exception is thrown.
	     */
	    void deserialize(data_t::const_iterator& pos, bool big_endian=false) throw(parse_error);


    };
}

#endif
