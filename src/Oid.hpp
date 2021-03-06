/*
 * Copyright 2011-2014 Tanjeff-Nicolai Moos <tanjeff@cccmz.de>
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

#include <ostream>
#include <string>

#include <QtGlobal>
#include <QVector>

#include "exceptions.hpp"


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
     * Oid myCompany("1.3.6.1.4.1.355");
     * \endcode
     *
     * Also a constructor is provided which takes an Oid and a string and
     * concatenates them, so this works also:
     *
     * \code
     * Oid myObject(myCompany, "1.1.3.0");
     * \endcode
     *
     * In addition, some common Oid's are provided as constants, e.g.
     * 'enterprises_oid', so the following will also work:
     *
     * \code
     * Oid yourCompany(enterprises_oid, 42); // 42 is an integer
     * Oid yourCompanyMib(enterprises_oid, "42.1.1"); // "42.1.1" is a string
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
     * This class inherits from QVector<quint32>, which means that an Oid
     * object can be manipulated the same way as a QVector<> can be
     * manipulated:
     *
     * \code
     * Oid theirCompany = enterprises_oid;
     * theirCompany.append(23);    // Don't use a string here!
     * \endcode
     *
     */
    class Oid: public QVector<quint32>
    {
	private:

            /**
             * \brief the 'include' field.
             */
            bool mInclude;

	    /**
	     * \brief Parse an OID from a string and append it.
	     *
	     * The OID contained within the string 's' is parsed and appended to
	     * this object. The format of the string is described above.
	     *
	     * \param s The OID to be parsed.
	     *
	     * \exception inval_param If the string is malformed.
	     */
	    void parseString(std::string s);

	public:

	    /**
	     * \brief Initialize an Oid object with an OID in string format.
	     *
	     * This constructor takes a string and initializes the Oid object
	     * with the OID contained within this string. The format of the 
	     * string is described above.
	     * 
	     * \param id The initial object identifier.
	     *
	     * \exception inval_param If the string is malformed.
	     */
	    Oid(std::string id = "");

	    /**
	     * \brief Initialize an Oid object with another Oid plus
	     *        a subid.
	     *
             * All subid's are copied from 'o'.  Then, the OID contained within 
             * the string 'id' is appended.  The format of the string is 
             * described in the documentation of this class.
             *
	     * \param o The starting OID.
	     *
             * \param id The OID to append.
	     *
	     * \exception inval_param If the string is malformed.
	     */
	    Oid(const Oid& o, std::string id);

            /**
             * \brief Initialize an Oid object with another Oid plus
             *        a subid.
             *
             * All subid's are copied from 'o'.  Then, the subid 'id' is
             * appended.
             *
             * \param o The starting OID.
             *
             * \param id The subid to append.
             *
             * \exception None.
             */
	    Oid(const Oid& o, quint32 id);

	    /**
	     * \brief Assignment operator
             *
             * Copies all data from 'o' into this OID.
             *
             * \param o The OID to copy from.
             *
             * \return A reference to this OID.
	     */
	    Oid& operator=(const Oid& o);

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
             *
             * \param o The OID to compare to.
             *
             * \return True if the OID is less than 'o', false otherwise.
	     */
	    bool operator<(const Oid& o) const;

	    /**
	     * \brief The less-than-or-equal operator
	     *
	     * See operator<() for a more detailed description about comparing
	     * OIDs.
	     *
	     * \param o The OID to compare to.
	     *
	     * \return True if the OID is less than or equal 'o',
	     *         false otherwise.
	     */
	    bool operator<=(const Oid& o) const
            {
	        if(*this == o || *this < o)
	        {
	            return true;
	        }
	        else
	        {
	            return false;
	        }
            }

	    /**
	     * \brief The equal-operator
	     *
	     * See operator<() for a more detailed description about comparing 
	     * OIDs.
             *
             * \param o The OID to compare to.
             *
             * \return True if the OIDs are equal, false otherwise.
	     */
	    bool operator==(const Oid& o) const;

	    /**
	     * \brief The not-equal-operator for OIDs.
	     *
	     * See operator<() for a more detailed description about comparing 
	     * OIDs.
             *
             * \param o The OID to compare to.
             *
             * \return False if the OIDs are equal, true otherwise.
	     */
	    bool operator!=(const Oid& o) const
	    {
		return ! (*this == o);
	    }

	    /**
	     * \brief The greater-than operator.
	     *
	     * See operator<() for a more detailed description about comparing 
	     * OIDs.
             *
             * \param o The OID to compare to.
             *
             * \return True if the OID is greater than 'o', false otherwise.
	     */
	    bool operator>(const Oid& o) const
	    {
		// a > b is the same as b < a :-)
		return o < *this;
	    }

            /**
             * \brief The greater-than-or-equal operator.
             *
             * See operator<() for a more detailed description about comparing
             * OIDs.
             *
             * \param o The OID to compare to.
             *
             * \return True if the OID is greater than or equal 'o',
             *         false otherwise.
             */
            bool operator>=(const Oid& o) const
            {
                // a >= b is the same as b <= a :-)
                return o <= *this;
            }

            /**
             * \brief Concatenation.
             *
             * This operator returns a temporary Oid object which is
             * this OID plus 'subid' appended. Example:
             *
             * \code
             * Oid a("1.2.3");
             * Oid b = a + 4; // b will be 1.2.3.4
             * \endcode
             *
             * \param subid The subid to append.
             *
             * \return The temporary object.
             */
            Oid operator+(quint32 subid) const
            {
                Oid result(*this);
                result.append(subid);
                return result;
            }

            /**
             * \brief Concatenation
             *
             * This operator returns a temporary Oid object which is
             * this OID plus 'o' appended. Example:
             *
             * \code
             * Oid a("1.2.3");
             * Oid b("4.5.6");
             * Oid c = a + b; // c will be 1.2.3.4.5.6
             * \endcode
             *
             * \param o The OID to append.
             *
             * \return The temporary object.
             */
            Oid operator+(const Oid& o) const
            {
                Oid result(*this);
                result += o;
                return result;
            }

            /**
             * \brief Concatenation.
             *
             * This operator appends 'o' to this OID. Example:
             *
             * \code
             * Oid a("1.2.3");
             * Oid b("4.5.6");
             * a += b;
             * // a is now 1.2.3.4.5.6
             * \endcode
             *
             * \param o The OID to append.
             *
             * \return A reference to this OID.
             */
            Oid& operator+=(const Oid& o)
            {
                this->QVector<quint32>::operator+=(o);
                return *this;
            }

            /**
	     * \brief Checks whether the given Oid is in the subtree of this
	     *        Oid.
	     *
	     * This method checks whether the given OID is included in the 
	     * subtree which has this Oid as root.
	     *
	     * Examples:\n
             * \code
             * Oid id("1.3.6.1.4.1.42.3");
             * id.contains( Oid("1.3.6.1.4.1.42.3") ); // true
             * id.contains( Oid("1.3.6.1.4.1.42") ); // false
             * id.contains( Oid("1.3.6.1.4.1.43.3") ); // false
             * id.contains( Oid("1.3.6.1.4.1.42.3.3.1") ); // true
             * \endcode
	     *
	     * \param id The OID to check.
	     *
	     * \return True if id is contained in the subtree, false
	     *         otherwise.
	     */
	    bool contains(const Oid& id) const;

	    /*
             * \internal
             *
	     * \brief Whether it is the null Object Identifier
	     *
	     * According to RFC 2741, 5.1 "Object Identifier", a null object 
	     * identifier has serial representation of 4 bytes which are 
	     * all set to 0. An OID with no subid's and the index field set to 
	     * 0 results in that representation and is thus considered to be 
	     * the null OID.
	     *
	     * \return True if the object is the null OID, false otherwise.
	     */
	    bool is_null() const;

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
	    bool include() const
	    {
	        return mInclude;
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
	    void setInclude(bool i)
	    {
	        mInclude = i;
	    }

	    friend std::ostream& operator<<(std::ostream& out,
					    const Oid& o);
    };

    /**
     * \brief The output operator for the Oid class.
     *
     * Object identifiers (Oid objects) can be output as follows:
     * 
     * \code
     * Oid led_state(enterprises_oid, "1.3.3.1");
     * cout << "LED state OID: " << led_state << endl;
     * \endcode
     *
     * The last line will output "LED state OID: .1.3.6.1.4.1.3.3.1".
     *
     * \param out The stream to which to write the output.
     *
     * \param o The OID to output.
     *
     * \return The 'out' parameter.
     */
    std::ostream& operator<<(std::ostream& out, const agentxcpp::Oid& o);


    // TODO: Possibly these should be put into the agentxcpp::Oid namespace?
    // The use of \memberof is not elegant.

    /**
     * \memberof Oid
     *
     * \brief The 'iso' OID according to RFC 1155.
     */
    const Oid iso_oid("1");

    /**
     * \memberof Oid
     *
     * \brief The 'ccitt' OID according to RFC 1155.
     */
    const Oid ccitt_oid("0");

    /**
     * \memberof Oid
     *
     * \brief The 'joint.iso.ccitt' OID according to RFC 1155.
     */
    const Oid joint_iso_ccitt_oid("2");

    /**
     * \memberof Oid
     *
     * \brief The 'iso.org' OID according to RFC 1155.
     */
    const Oid org_oid(iso_oid,"3");

    /**
     * \memberof Oid
     *
     * \brief The 'iso.org.dod' OID according to RFC 1155.
     */
    const Oid dod_oid(org_oid,"6");

    /**
     * \memberof Oid
     *
     * \brief The 'iso.org.dod.internet' OID according to RFC 1155.
     */
    const Oid internet_oid(dod_oid,"1");

    /**
     * \memberof Oid
     *
     * \brief The 'iso.org.dod.internet.directory' OID according to RFC 1155.
     */
    const Oid directory_oid(internet_oid,"1");

    /**
     * \memberof Oid
     *
     * \brief The 'iso.org.dod.internet.mgmt' OID according to RFC 1155.
     */
    const Oid mgmt_oid(internet_oid,"2");

    /**
     * \memberof Oid
     *
     * \brief The 'iso.org.dod.internet.experimental' OID according to
     *        RFC 1155.
     */
    const Oid experimental_oid(internet_oid,"3");

    /**
     * \memberof Oid
     *
     * \brief The 'iso.org.dod.internet.private' OID according to RFC 1155.
     */
    const Oid private_oid(internet_oid,"4");

    /**
     * \memberof Oid
     *
     * \brief The 'iso.org.dod.internet.private.enterprises' OID according to
     *        RFC 1155.
     */
    const Oid enterprises_oid(private_oid, "1");

    /**
     * \memberof Oid
     *
     * \brief The 'snmpMIBObjects' OID according to RFC 1907.
     *
     * \internal
     *
     * This OID is used in the context of notifications.
     *
     * According to RFC 1907:<br/>
     * snmpTrapOID ::= snmpTrap.1<br/>
     * snmpTrap ::= snmpMIBObjects.4<br/>
     * snmpMIBObjects ::= snmpMIB.1<br/>
     * snmpMIB ::= snmpModules.1<br/>
     *
     * According to RFC 2578 (SNMPv2-SMI):<br/>
     * snmpModules ::= snmpV2.3<br/>
     * snmpV2 ::= internet.6
     *
     * Conclusion:<br/>
     * snmpMIBObjects ::= internet.6.3.1.1
     */
    const Oid snmpMIBObjects_oid(internet_oid, "6.3.1.1");

    /**
     * \memberof Oid
     *
     * \brief The 'snmpTrapOID' OID according to RFC 1907.
     *
     * \internal
     *
     * This OID is used in the context of notifications.
     *
     * According to RFC 1907:<br/>
     * snmpTrapOID ::= snmpTrap.1<br/>
     * snmpTrap ::= snmpMIBObjects.4<br/>
     *
     * Conclusion:<br/>
     * snmpTrapOID ::= snmpMIBObjects.4.1
     */
    const Oid snmpTrapOID_oid(snmpMIBObjects_oid, "4.1");

    /**
     *
     * \memberof Oid
     *
     * \brief The sysUpTime_oid OID according to RFC 1907.
     *
     * \internal
     *
     * This OID is used in the context of notifications.
     *
     * According to RFC 1907:<br/>
     * sysUpTime = system.3<br/>
     * system = mib-2.1
     *
     * According to RFC 1902:<br/>
     * mib-2 = mgmt.1OidValue
     *
     * Conclusion:
     * sysUpTime = mgmt.1.1.3
     */
    const Oid sysUpTime_oid(mgmt_oid, "1.1.3");
}


#endif  // _OID_H_
