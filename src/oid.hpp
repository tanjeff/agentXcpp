/*
 * Copyright 2011 Tanjeff-Nicolai Moos <tanjeff@cccmz.de>
 *
 * This file is part of the agentXcpp library.
 *
 * AgentXcpp is free software: you can redistribute it and/or modify
 * it under the terms of the AgentXcpp library license, version 1, which 
 * consists of the GNU General Public License and some additional 
 * permissions.
 *
 * AgentXcpp is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * See the AgentXcpp library license in the LICENSE file of this package 
 * for more details.
 */

#ifndef _OID_H_
#define _OID_H_

#include <vector>
#include <ostream>
#include "variable.hpp"
#include "types.hpp"

namespace agentxcpp
{
    /**
     * \brief Represents an SNMP object identifier (OID)
     *
     * This class represents an OID. OID's are sequences of integers. This 
     * class inherits from std:vector<>, which means that an oid object can be 
     * manipulated much the same way as a std::vector<> can be manipulated.
     *
     * In addition, the OID class provides constructors taking a number of 
     * integers to ease creatng such objects. For example, this works:
     *
     * \code
     * oid myCompany = oid(1,3,6,1,4,1,355);
     * \endcode
     *
     * Also a constructor is provided which takes an oid and a number of 
     * integers, so this works also:
     *
     * \code
     * oid myObject = oid(myCompany,1,1,3,42);
     * \endcode
     *
     * In addition, some common oid's are provided as constants, e.g. 
     * 'enterprises', so the following will also work:
     *
     * \code
     * oid myCompany = oid(enterprises,355);
     * \endcode
     */
    class oid: public variable, public std::vector<uint32_t>
    {
	private:

	    /**
	     * \brief the 'include' field.
	     */
	    bool include;

	public:

	    /**
	     * \brief Initialize an oid object with a sequence of up to 20
	     *        subidentifiers.
	     *
	     * This constructor takes up to 20 subidentifiers which forms up 
	     * the object identifier. More subidentifiers can be added using 
	     * functions from the vector<> class.
	     *
	     * The 'include' field is initialized to 'false'.
	     *
	     * This is also the default constructor.
	     *
	     * \note Zero (0) is not allowed for a subidentifier, i.e. the
	     *       first subidentifier with value 0 and all its successors 
	     *       are ignored.
	     */
	    oid(uint32_t  c1=0, uint32_t  c2=0, uint32_t  c3=0,
		uint32_t  c4=0,	uint32_t  c5=0, uint32_t  c6=0,
		uint32_t  c7=0, uint32_t  c8=0, uint32_t  c9=0,
		uint32_t c10=0, uint32_t c11=0, uint32_t c12=0,
		uint32_t c13=0, uint32_t c14=0, uint32_t c15=0,
		uint32_t c16=0, uint32_t c17=0, uint32_t c18=0,
		uint32_t c19=0, uint32_t c20=0 );

	    /**
	     * \brief Initialize an oid object with another oid plus a sequence 
	     * of up to 19 subidentifiers.
	     *
	     * The 'include' field is copied from o.
	     *
	     * This is also the copy constructor.
	     *
	     * \note Zero (0) is not allowed for a subidentifier, i.e. the
	     *       first subidentifier with value 0 and all its successors 
	     *       are ignored.
	     */
	    oid(const oid& o,
		uint32_t  c1=0, uint32_t  c2=0, uint32_t  c3=0,
		uint32_t  c4=0, uint32_t  c5=0, uint32_t  c6=0,
		uint32_t  c7=0, uint32_t  c8=0,	uint32_t  c9=0,
		uint32_t c10=0, uint32_t c11=0, uint32_t c12=0,
		uint32_t c13=0, uint32_t c14=0, uint32_t c15=0,
		uint32_t c16=0, uint32_t c17=0, uint32_t c18=0,
		uint32_t c19=0 );

	    /**
	     * \brief Assignment operator
	     */
	    oid& operator=(const oid& oid);


	    /**
	     * \internal
	     *
	     * \brief get the current include value
	     *
	     * The include value is present in the serialized form of an OID.  
	     * If an OID object is created by parsing a AgentX message, the 
	     * 'include' member is set accordingly.
	     *
	     * See RFC 2741, sections 5.1 and 5.2 for details.
	     */
	    bool get_include()
	    {
		return include;
	    }

	    /**
	     * \internal
	     *
	     * \brief set the include value
	     *
	     * The include value is present in the serialized form of an OID.  
	     * If an OID object is serialized, the include field is encoded 
	     * into the stream.
	     *
	     * See RFC 2741, sections 5.1 and 5.2 for details.
	     */
	    void set_include(bool i)
	    {
		include = i;
	    }

	    /**
	     * \internal
	     *
	     * \brief Encode an OID object as described in RFC 2741,
	     *        section 5.1.
	     */
	    data_t serialize() const;

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
	     * fails, parse_error is thrown. In this case, the iterator is left 
	     * at an undefined position.
	     *
	     * \param pos Iterator pointing to the current stream position.
	     *            The iterator is advanced while reading the header.
	     *
	     * \param end Iterator pointing one element past the end of the
	     *            current stream. This is needed to mark the end of the 
	     *            buffer.
	     *
	     * \param big_endian Whether the input stream is in big endian
	     *                   format
	     *
	     * \exception parse_error If parsing fails. In this case, the
	     *                        iterator is left at an undefined 
	     *                        position.
	     */
	    oid(data_t::const_iterator& pos,
		const data_t::const_iterator& end,
		bool big_endian=true);

	    /**
	     * \brief The less-than operator
	     *
	     * An OID is less than another OID if either the first 
	     * not-identical part is lesser or if all parts are identical and 
	     * it has lesser parts.
	     *
	     * Example:\n
	     * 1.3.6.1.4.1.42.3<b>.3</b>.1 \n
	     * is less than \n
	     * 1.3.6.1.4.1.42.3<b>.4</b>.1 \n
	     * Note the next to last number.
	     *
	     * Also,\n
	     * 1.3.6.1.4.1.42.3.3.1 \n
	     * is less than \n
	     * 1.3.6.1.4.1.42.3.3.1<b>.1</b> \n
	     * because it is shorter.
	     *
	     * However, \n
	     * 1.3.6.1.4.1.42.3<b>.3</b>.1 \n
	     * is greater than \n
	     * 1.3.6.1.4.1.42.3<b>.2</b>.1.1 \n
	     * because the 9th number is greater (although the first OID has 
	     * less numbers than the second).
	     */
	    bool operator<(const oid& o) const;

	    /**
	     * \brief The equal-operator
	     *
	     * See operator<() for a more detailed description about comparing 
	     * OIDs.
	     */
	    bool operator==(const oid& o) const;

	    /**
	     * \brief The not-equal-operator for oids
	     *
	     * See operator<() for a more detailed description about comparing 
	     * OIDs.
	     */
	    bool operator!=(const oid& o) const
	    {
		return ! (*this == o);
	    }

	    /**
	     * \brief The greater-than operator
	     *
	     * See operator<() for a more detailed description about comparing 
	     * OIDs.
	     */
	    bool operator>(const oid& o) const
	    {
		// a > b is the same as b < a :-)
		return o < *this;
	    }

	    /**
	     * \brief Checks whether the given oid is in the subtree of this
	     *        oid.
	     *
	     * This method checks whether the given OID is included in the 
	     * subtree which has this oid as root.
	     *
	     * Examples:\n
	     * oid id(1.3.6.1.4.1.42.3); \n
	     * id.contains( oid(1.3.6.1.4.1.42.3) ); // true \n
	     * id.contains( oid(1.3.6.1.4.1.42) ); // false \n
	     * id.contains( oid(1.3.6.1.4.1.43.3) ); // false \n
	     * id.contains( oid(1.3.6.1.4.1.42.3.3.1) ); // true \n
	     *
	     * \param id The OID to check.
	     *
	     * \return True if id is contained in the subtree, false
	     *         otherwise.
	     */
	    bool contains(const oid& id);

	    /**
	     * \brief Whether it is the null Object Identifier
	     *
	     * According to RFC 2741, 5.1 "Object Identifier", a null object 
	     * identifier has serial representation of for 4 bytes which are 
	     * all set to 0. An OID with no subid's and the index field set to 
	     * 0 results in that representation and is thus considered as null 
	     * OID.
	     *
	     * \return True if the object is the null OID, false otherwise.
	     */
	    bool is_null() const;

	    friend std::ostream& operator<<(std::ostream&,
					    const agentxcpp::oid&);
    };

    /**
     * \brief The output operator for the oid class.
     *
     * Object identifiers (oid objects) can be output as follows:
     * 
     * \code
     * oid led_state(enterprise,1,3,3,1);
     * cout << "LED state OID: " << led_state << endl;
     * \endcode
     *
     * The last line will output "LED state OID:  .1.3.6.1.4.1.3.3.1".
     */
    std::ostream& operator<<(std::ostream&, const agentxcpp::oid&);


    // Some oid's according to RFC 1155:
    const oid iso(1);
    const oid ccitt(0);
    const oid joint_iso_ccitt(2);
    const oid org(iso,3);
    const oid dod(org,6);
    const oid internet(dod,1);
    const oid directory(internet,1);
    const oid mgmt(internet,2);
    const oid experimental(internet,3);
    // 'private' is a C++ keyword, thus we use private_:
    const oid private_(internet,4);
    const oid enterprises(private_, 1);
}


#endif
