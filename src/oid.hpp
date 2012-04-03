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
#include <string>
#include "variable.hpp"
#include "types.hpp"

namespace agentxcpp
{
    /**
     * \brief Represents an SNMP object identifier (OID).
     *
     * This class represents an OID. OID's are sequences of sub-identifiers, 
     * which are integers.
     *
     * This class provides constructors taking a string which contains an OID.  
     * For example, this works:
     *
     * \code
     * oid myCompany = oid("1.3.6.1.4.1.355");
     * \endcode
     *
     * Also a constructor is provided which takes an oid and a string and 
     * concatenates them, so this works also:
     *
     * \code
     * oid myObject = oid(myCompany, "1.1.3.0");
     * \endcode
     *
     * In addition, some common oid's are provided as constants, e.g.  
     * 'enterprises', so the following will also work (note that the second 
     * argument is a string, not an integer!):
     *
     * \code
     * oid yourCompany = oid(enterprises, "42"); // second param is a string!
     * \endcode
     *
     * The string given to the constructors must have a valid syntax. If a 
     * malformed string is provided, inval_param is thrown and the object is 
     * not constructed. For example, the following strings are malformed:
     *
     * \code
     * "1,3,6"  // wrong separator (must be a period)
     * "1.3.6." // trailing character at the end
     * "1.3.6.1.4.1.123456789123" // last subid is too big (must fit in a 32 bit unsigned integer)
     * \endcode
     *
     * However, the following strings are accepted:
     *
     * \code
     * "1.3.6"
     * "1"  // a single subid is ok
     * "1.3.6.1.4.1.42.1.0" // 0 as subid is ok
     * ""   // empty string is ok
     * \endcode
     *
     * This class inherits from std:vector<uint32_t>, which means that an oid 
     * object can be manipulated the same way as a std::vector<> can be 
     * manipulated:
     *
     * \code
     * oid theirCompany = enterprises;
     * theirCompany.push_back(23);    // Don't use a string here!
     * \endcode
     *
     */
    class oid: public variable, public std::vector<uint32_t>
    {
	private:

	    /**
	     * \brief the 'include' field.
	     */
	    bool include;

	    /**
	     * \brief Parse an OID from a string and append it.
	     *
	     * The OID contained within the string 's' is parsed and appended 
	     * this object. The format of the string is described above 
	     *
	     * \param s The OID to be parsed.
	     *
	     * \exception inval_param If the string is malformed.
	     */
	    void parse_string(std::string s);

	public:

	    /**
	     * \brief Default Constructor
	     *
	     * This constructs an empty oid (the null oid). The 'include' field 
	     * is initialized to 'false'.
	     *
	     * \exception None.
	     */
	    oid()
	    {
		include = false;
	    }


	    /**
	     * \brief Initialize an oid object with an OID in string format.
	     *
	     * This constructor takes a string and initializes the oid object 
	     * with the OID contained within this string. The format of the 
	     * string is described above.
	     * 
	     * The 'include' field is initialized to 'false'.
	     *
	     * \param id The initial object identifier.
	     *
	     * \exception inval_param If the string is malformed.
	     */
	    oid(std::string id);

	    /**
	     * \brief Initialize an oid object with another oid plus an OID in 
	     * string format.
	     *
	     * The 'include' field and all subid's are copied from 'o'. Then, 
	     * the OID contained within the string 'id' is appended.  The 
	     * format of the string is described in the documentation of this 
	     * class.
	     *
	     * \param o The starting OID.
	     *
	     * \param id The OID to append to o.
	     *
	     * \exception inval_param If the string is malformed.
	     */
	    oid(const oid& o, std::string id);

	    /**
	     * \brief Assignment operator
	     */
	    oid& operator=(const oid& oid);

	    /**
	     * \internal
	     *
	     * \brief Get the current include value.
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
	     * 0 results in that representation and is thus considered to be 
	     * the null OID.
	     *
	     * \return True if the object is the null OID, false otherwise.
	     */
	    bool is_null() const;

	    friend std::ostream& operator<<(std::ostream&,
					    const agentxcpp::oid&);

	    /**
	     * \brief Update the internal state of the object.
	     *
	     * This function calls get() to obtain a new value and stores that 
	     * value within this object.
	     *
             * \exception generic_error If obtaining the new value failed.
	     */
	    virtual void update()
	    {
		//*this = this->get();
	    }
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
    // TODO: document them! Possibly these should be put into the 
    // agentxcpp::oid namespace?
    const oid iso("1");
    const oid ccitt("0");
    const oid joint_iso_ccitt("2");
    const oid org(iso,"3");
    const oid dod(org,"6");
    const oid internet(dod,"1");
    const oid directory(internet,"1");
    const oid mgmt(internet,"2");
    const oid experimental(internet,"3");
    // 'private' is a C++ keyword, thus we use private_:
    const oid private_(internet,"4");
    const oid enterprises(private_, "1");
}


#endif
