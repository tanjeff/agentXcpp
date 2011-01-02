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
	     * \brief Construct the object from input stream
	     *
	     * This constructor parses the serialized form of the object.
	     * It takes an iterator, starts parsing at the position of the 
	     * iterator and advances the iterator to the position right behind 
	     * the object.
	     * 
	     * The constructor expects valid data from the stream; if parsing 
	     * fails, parse_error is thrown. In this case, the iterator 
	     * position is undefined.
	     *
	     * \param pos iterator pointing into the stream
	     * \param big_endian Whether the input stream is in big endian
	     *                   format
	     */
	    IpAddress(data_t::const_iterator& pos, bool big_endian=true) throw(parse_error);

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
    };
}

#endif
