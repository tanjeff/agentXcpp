#ifndef __REGISTERPDU_H__
#define __REGISTERPDU_H__


namespace agentx
{
    class RegisterPDU : public PDU
    {
	private:
	    byte_t timeout;
	    byte_t priority;
	    byte_t range_subid;
	    oid upper_bound;

	public:
	    /**
	     * \brief Parse constructor
	     *
	     * Construct the object by parsing a stream. See \ref parsing for 
	     * details about %PDU parsing.
	     *
	     * \param pos Iterator pointing to the current stream position.
	     *            The iterator is advanced while reading the header.
	     *
	     * \param big_endian Whether the serialized form of the %PDU is
	     *                   in big_endian format.
	     *
	     * \exception parse_error If parsing fails, for example because
	     *			      reading the stream fails or the %PDU is 
	     *			      malformed.
	     */
	    RegisterPDU(data_t::const_iterator& pos, bool big_endian) throw(parse_error);
    };
}
#endif
