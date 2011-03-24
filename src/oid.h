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
#include "variable.h"
#include "types.h"
#include "exceptions.h"

namespace agentxcpp
{
    /**
     * \brief Represents an SNMP object identifier (OID)
     */
    class oid: public variable
    {
	std::vector<uint32_t> identifier;
	bool include;

	public:

	/**
	 * \brief Initialize an oid object with a sequence of up to 20 
	 * intergers.
	 *
	 * This constructor takes up to 20 integers which forms up the object 
	 * identifier. If you need more than 20 components, use 
	 * oid::operator,() .
	 */
	// TODO: How wide are the integers for OIDs?
	oid(int c1=0, int c2=0, int c3=0, int c4=0, int c5=0, int c6=0,
	    int c7=0, int c8=0, int c9=0, int c10=0, int c11=0, int c12=0,
	    int c13=0, int c14=0, int c15=0, int c16=0, int c17=0, int c18=0,
	    int c19=0, int c20=0 );
	
	/**
	 * \brief Append integers to an oid object.
	 *
	 * The ',' operator is provided to append integers to an oid object.  
	 * The following example will create the oid 1.3.6.1:
	 *
	 * \code
	 * oid led_state(1);
	 * led_state = led_state, 3;
	 * led_state = led_state, 6;
	 * led_state = led_state, 1;
	 * \endcode
	 * 
	 * A more useful example would be:
	 * 
	 * \code
	 * oid led_state;
	 * led_state = 1, 3, 6, 1;
	 * \endcode
	 * 
	 * In the last example, the assignment is evaluated before the ',' 
	 * operators (due to the C++ operator precedence rules). Therefore the 
	 * literal 1 is converted to an oid object which is then assigned to 
	 * led_state. After that the ',' is invoked on led_state several times.
	 *
	 * Note that the following will NOT work:
	 * 
	 * \code
	 * oid led_state = 1, 3, 6, 1;
	 * \endcode
	 *
	 * To understand why it does not work, consider this code:
	 *
	 * \code
	 * oid led_state = 1, fan_speed = 3, cpu_usage = 6, hdd_temp = 1;
	 * \endcode
	 */
	oid& operator,(int);

	/**
	 * \brief get the current include value
	 *
	 * The include value is present in the serialized form of an OID. If an 
	 * OID object is created by parsing a AgentX message, the 'include' 
	 * member is set accordingly.
	 *
	 * See RFC 2741, sections 5.1 and 5.2 for details.
	 */
	bool get_include() { return include; }

	/**
	 * \brief set the include value
	 *
	 * The include value is present in the serialized form of an OID. If an 
	 * OID object is serialized, the include field is encoded into the 
	 * stream.
	 *
	 * See RFC 2741, sections 5.1 and 5.2 for details.
	 */
	void set_include(bool i) { include = i; }

	/**
	 * \internal
	 *
	 * \brief Encode an OID object as described in RFC 2741, section 5.1.
	 */
	data_t serialize();

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
	oid(data_t::const_iterator& pos, bool big_endian=true) throw(parse_error);

	/**
	 * \brief The less-than operator
	 *
	 * An OID is less than another OID if the first not-identical part is 
	 * lesser or, if all parts are identical, it has lesser parts.
	 *
	 * Example:\n
	 * 1.3.6.1.4.1.42.3.3.1 \n
	 * is less than \n
	 * 1.3.6.1.4.1.42.3.4.1 \n
	 * Note the next to last number.
	 *
	 * Also,\n
	 * 1.3.6.1.4.1.42.3.3.1 \n
	 * is less than \n
	 * 1.3.6.1.4.1.42.3.3.1.1 \n
	 * because it is shorter.
	 *
	 * However, \n
	 * 1.3.6.1.4.1.42.3.3.1 \n
	 * is greater less than \n
	 * 1.3.6.1.4.1.42.3.2.1.1 \n
	 * because the 9th number is greater (although the first OID has less 
	 * numbers than the second).
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
    
	friend std::ostream& operator<<(std::ostream&, const agentxcpp::oid&);

    };
    
    /**
     * \brief The output operator for the oid class.
     *
     * Object identifiers (oid objects) can be output as follows:
     * 
     * \code
     * oid led_state(1,3,6,1);
     * cout << "LED state OID: " << led_state << endl;
     * \endcode
     *
     * The last line will output "LED state OID: 1.3.6.1".
     */
    std::ostream& operator<<(std::ostream&, const agentxcpp::oid&);

}


#endif
