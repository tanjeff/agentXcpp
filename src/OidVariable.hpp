/*
 * Copyright 2011-2012 Tanjeff-Nicolai Moos <tanjeff@cccmz.de>
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

#ifndef _OIDVARIABLE_H_
#define _OIDVARIABLE_H_

#include <ostream>
#include <string>

#include <QtGlobal>
#include <QVector>

#include "AbstractVariable.hpp"
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
     * OidValue myCompany = OidValue("1.3.6.1.4.1.355");
     * \endcode
     *
     * Also a constructor is provided which takes an OidValue and a string and 
     * concatenates them, so this works also:
     *
     * \code
     * OidValue myObject = OidValue(myCompany, "1.1.3.0");
     * \endcode
     *
     * In addition, some common OidValue's are provided as constants, e.g.  
     * 'enterprises_oid', so the following will also work (note that the second 
     * argument is a string, not an integer!):
     *
     * \code
     * OidValue yourCompany = OidValue(enterprises_oid, "42"); // second param is a string!
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
     * This class inherits from std:vector<quint32>, which means that an OidValue 
     * object can be manipulated the same way as a std::vector<> can be 
     * manipulated:
     *
     * \code
     * OidValue theirCompany = enterprises_oid;
     * theirCompany.push_back(23);    // Don't use a string here!
     * \endcode
     *
     */
    class OidVariable: public AbstractVariable, public QVector<quint32>
    {
	private:

	    /**
	     * \brief the 'include' field.
	     */
	    bool include;

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

            /**
             * \brief The new value for the variable in a Set operation.
             *
             * The Set operation is performed in up to four steps (testset,
             * commitset, cleanupset, undoset). Only the testset step actually
             * receives the new value. This value is stored here so that it can
             * be delivered to commitset(), undoset() and cleanupset().
             */
            QSharedPointer<OidVariable> new_value;

	public:

	    /**
	     * \brief Default Constructor
	     *
	     * This constructs an empty OidValue (the null OidValue).
             *
             * \internal
             * The 'include' field is initialized to 'false'.
             * \endinternal
	     *
	     * \exception None.
	     */
	    OidVariable()
	    {
		include = false;
	    }


	    /**
	     * \brief Initialize an OidValue object with an OID in string format.
	     *
	     * This constructor takes a string and initializes the OidValue object 
	     * with the OID contained within this string. The format of the 
	     * string is described above.
	     * 
             * \internal
             * The 'include' field is initialized to 'false'.
             * \endinternal
	     *
	     * \param id The initial object identifier.
	     *
	     * \exception inval_param If the string is malformed.
	     */
	    OidVariable(std::string id);

	    /**
	     * \brief Initialize an OidValue object with another OidValue plus an OID in 
	     * string format.
	     *
             * All subid's are copied from 'o'.  Then, the OID contained within 
             * the string 'id' is appended.  The format of the string is 
             * described in the documentation of this class.
             *
             * \internal
             * The 'include' field is copied from 'o'.
             * \endinternal
	     *
	     * \param o The starting OID.
	     *
             * \param id The OID to append.
	     *
	     * \exception inval_param If the string is malformed.
	     */
	    OidVariable(const OidVariable& o, std::string id);

	    /**
	     * \brief Assignment operator
             *
             * Copies all data from 'o' into this OID.
             *
             * \param o The OID to copy from.
             *
             * \return A reference to this OID.
	     */
	    OidVariable& operator=(const OidVariable& o);

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
	    bool get_include() const
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
	    binary serialize() const;

	    /**
	     * \internal
	     *
	     * \brief Parse Constructor.
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
	    OidVariable(binary::const_iterator& pos,
	                const binary::const_iterator& end,
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
             *
             * \param o The OID to compare to.
             *
             * \return True if the OID is less than 'o', false otherwise.
	     */
	    bool operator<(const OidVariable& o) const;

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
	    bool operator<=(const OidVariable& o) const
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
             * \param o The OID tocompare to.
             *
             * \return True if the OIDs are equal, false otherwise.
	     */
	    bool operator==(const OidVariable& o) const;

	    /**
	     * \brief The not-equal-operator for OidValues
	     *
	     * See operator<() for a more detailed description about comparing 
	     * OIDs.
             *
             * \param o The OID tocompare to.
             *
             * \return False if the OIDs are equal, true otherwise.
	     */
	    bool operator!=(const OidVariable& o) const
	    {
		return ! (*this == o);
	    }

	    /**
	     * \brief The greater-than operator
	     *
	     * See operator<() for a more detailed description about comparing 
	     * OIDs.
             *
             * \param o The OID tocompare to.
             *
             * \return True if the OID is greater than 'o', false otherwise.
	     */
	    bool operator>(const OidVariable& o) const
	    {
		// a > b is the same as b < a :-)
		return o < *this;
	    }

            /**
             * \brief The greater-than-or-equal operator
             *
             * See operator<() for a more detailed description about comparing
             * OIDs.
             *
             * \param o The OID to compare to.
             *
             * \return True if the OID is greater than or equal 'o',
             *         false otherwise.
             */
            bool operator>=(const OidVariable& o) const
            {
                // a >= b is the same as b <= a :-)
                return o <= *this;
            }

            /**
	     * \brief Checks whether the given OidValue is in the subtree of this
	     *        OidValue.
	     *
	     * This method checks whether the given OID is included in the 
	     * subtree which has this OidValue as root.
	     *
	     * Examples:\n
             * \code
             * OidValue id("1.3.6.1.4.1.42.3");
             * id.contains( OidValue(1.3.6.1.4.1.42.3) ); // true
             * id.contains( OidValue(1.3.6.1.4.1.42) ); // false
             * id.contains( OidValue(1.3.6.1.4.1.43.3) ); // false
             * id.contains( OidValue(1.3.6.1.4.1.42.3.3.1) ); // true
             * \endcode
	     *
	     * \param id The OID to check.
	     *
	     * \return True if id is contained in the subtree, false
	     *         otherwise.
	     */
	    bool contains(const OidVariable& id);

	    /**
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

	    friend std::ostream& operator<<(std::ostream& out,
					    const OidVariable& o);

	    /**
	     * \brief Convert the value to an OID.
	     *
	     * The conversion is done according to RFC 2578,
	     * 7.7. "Mapping of the INDEX clause". First, the
	     * length is converted to a subid. Then, each
	     * subid of the value is appended.
	     *
	     * Note that the length subid is omitted for fixed-length OIDs.
	     *
	     * \param fixedLength Whether the OID is fixed-length.
	     */
	    OidVariable toOid(bool fixedLength = false) const
	    {
	        OidVariable oid;

	        // Store length if needed
	        if(!fixedLength)
	        {
	            oid.push_back(this->size());
	        }

	        // Store value
	        oid += *this;

	        return oid;
	    }

            /**
             * \internal
             *
             * \brief Handle a Get Request.
             *
             * This function calls get() to obtain the new value,
             * converts it to QSharedPointer<AbstractValue> and returns it.
             */
            virtual QSharedPointer<AbstractVariable> handle_get()
            {
                this->get();
                return QSharedPointer<OidVariable>(new OidVariable(*this));
            }

            /**
             * \brief Handle a Get request.
             *
             * This method is called when an SNMP Get request is received.
             * It shall
             * return the current value of the variable.
             *
             * \note This method is pure virtual and thus \e must be
             *       implemented. It is not possible to implement write-only
             *       SNMP variables.
             *
             * \return The value of the variable.
             */
            virtual void get()
            {
            }

            /**
             * \internal
             *
             * \brief Handle a TestSet request.
             *
             * This function converts the argument to QSharedPointer<T>() and
             * calls
             * testset() with the converted value. If conversion fails,
             * testset() is not called. This function also stores the given
             * value to the new_value member.
             *
             * \param v The new value for the variable.
             *
             * \return agentxcpp::AbstractVariable::wrongType if the conversion
             *                                                fails. Otherwise,
             *                                                the result of
             *                                                testset() is
             *                                                returned.
             */
            virtual testset_result_t handle_testset(QSharedPointer<AbstractVariable> _v)
            {
                new_value = qSharedPointerDynamicCast<OidVariable>(_v);
                if (new_value)
                {
                    // Type matches variable
                    return testset(new_value);
                }
                else
                {
                    // Wrong type
                    return wrongType;
                }

            }

            /**
             * \brief Handle a TestSet request.
             *
             * This method is called when an SNMP TestSet request is received.
             * It
             * shall check whether a Set operation is possible for the
             * variable.  It shall acquire the resources needed to perform the
             * Set operation (but the Set shall not yet be performed).
             *
             * The default implementation returns
             * agentxcpp::AbstractVariable::noAccess to indicate that
             * this is a read-only variable. Thus, for read-only variables this
             * method need not be overridden.
             *
             * \param v The new value for the object.
             *
             * \return The result of the check.
             */
            virtual testset_result_t testset(QSharedPointer<OidVariable> _v)
            {
                return noAccess;
            }

            /**
             * \internal
             *
             * \brief Handle a CleanupSet request.
             *
             * This function calls cleanupset() with the value from the last
             * handle_testset() invokation.
             */
            virtual void handle_cleanupset()
            {
                cleanupset(new_value);
            }

            /**
             * \brief Handle a CleanupSet request.
             *
             * This method is called when an SNMP CleanupSet request is
             * received. It
             * shall release any ressources allocated by testset().
             *
             * The default implementation does nothing. If no action is
             * required to perform the CleanupSet operaiton, this method need
             * not be overridden.
             *
             * \param v The new value for the object.
             */
            virtual void cleanupset(QSharedPointer<OidVariable> _v)
            {
                return;
            }

            /**
             * \internal
             *
             * \brief Handle a CommitSet request.
             *
             * This function calls commitset() with the value from the last
             * handle_testset() Invocation and returns its return value.
             */
            virtual bool handle_commitset()
            {
                return commitset(new_value);
            }

            /**
             * \brief Handle a CommitSet request.
             *
             * This method is called when an SNMP CommitSet request is
             * received. It
             * shall perform the actual write operation.
             *
             * The default implementation returns false to indicate that the
             * operation failed. To implement a writable SNMP variable this
             * method must be overridden.
             *
             * \param v The new value for the object.
             *
             * \return True if the operation succeeded, false otherwise.
             */
            virtual bool commitset(QSharedPointer<OidVariable> _v)
            {
                return false;
            }

            /**
             * \internal
             *
             * \brief Handle a UndoSet request.
             *
             * This function calls undoset() with the value from the last
             * handle_testset() invocation and returns its return value.
             *
             */
            virtual bool handle_undoset()
            {
                return undoset(new_value);
            }

            /**
             * \brief Handle an UndoSet request.
             *
             * This method is called when an SNMP UndoSet request is received.
             * It
             * shall undo whatever commitset() performed. It shall also release
             * all resources allocated by testset().
             *
             * The default implementation returns false to indicate that
             * the operation failed. It is strongly recommended that writable
             * variables override this method.
             *
             * \internal
             *
             * A default implementation is provided to allow implementing
             * read-only variables.
             *
             * \endinternal
             *
             * \param v The new value for the object.
             *
             * \return True on success, false otherwise.
             */
            virtual bool undoset(QSharedPointer<OidVariable> _v)
            {
                return false;
            }

    };

    /**
     * \brief The output operator for the OidValue class.
     *
     * Object identifiers (OidValue objects) can be output as follows:
     * 
     * \code
     * OidValue led_state(enterprises_oid, "1.3.3.1");
     * cout << "LED state OID: " << led_state << endl;
     * \endcode
     *
     * The last line will output "LED state OID:  .1.3.6.1.4.1.3.3.1".
     *
     * \param out The stream to which to write the output.
     *
     * \param o The OID to output.
     *
     * \return The 'out' parameter.
     */
    std::ostream& operator<<(std::ostream& out, const agentxcpp::OidVariable& o);


    // TODO: Possibly these should be put into the agentxcpp::OidValue namespace?  
    // The use of \memberof is not elegant.

    /**
     * \memberof OidValue
     *
     * \brief The 'iso' OID according to RFC 1155.
     */
    const OidVariable iso_oid("1");

    /**
     * \memberof OidValue
     *
     * \brief The 'ccitt' OID according to RFC 1155.
     */
    const OidVariable ccitt_oid("0");

    /**
     * \memberof OidValue
     *
     * \brief The 'joint.iso.ccitt' OID according to RFC 1155.
     */
    const OidVariable joint_iso_ccitt_oid("2");

    /**
     * \memberof OidValue
     *
     * \brief The 'iso.org' OID according to RFC 1155.
     */
    const OidVariable org_oid(iso_oid,"3");

    /**
     * \memberof OidValue
     *
     * \brief The 'iso.org.dod' OID according to RFC 1155.
     */
    const OidVariable dod_oid(org_oid,"6");

    /**
     * \memberof OidValue
     *
     * \brief The 'iso.org.dod.internet' OID according to RFC 1155.
     */
    const OidVariable internet_oid(dod_oid,"1");

    /**
     * \memberof OidValue
     *
     * \brief The 'iso.org.dod.internet.directory' OID according to RFC 1155.
     */
    const OidVariable directory_oid(internet_oid,"1");

    /**
     * \memberof OidValue
     *
     * \brief The 'iso.org.dod.internet.mgmt' OID according to RFC 1155.
     */
    const OidVariable mgmt_oid(internet_oid,"2");

    /**
     * \memberof OidValue
     *
     * \brief The 'iso.org.dod.internet.experimental' OID according to
     *        RFC 1155.
     */
    const OidVariable experimental_oid(internet_oid,"3");

    /**
     * \memberof OidValue
     *
     * \brief The 'iso.org.dod.internet.private' OID according to RFC 1155.
     */
    const OidVariable private_oid(internet_oid,"4");

    /**
     * \memberof OidValue
     *
     * \brief The 'iso.org.dod.internet.private.enterprises' OID according to
     *        RFC 1155.
     */
    const OidVariable enterprises_oid(private_oid, "1");

    /**
     * \memberof OidValue
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
    const OidVariable snmpMIBObjects_oid(internet_oid, "6.3.1.1");

    /**
     * \memberof OidValue
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
    const OidVariable snmpTrapOID_oid(snmpMIBObjects_oid, "4.1");

    /**
     *
     * \memberof OidValue
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
    const OidVariable sysUpTime_oid(mgmt_oid, "1.1.3");

}


#endif  // _OIDVARIABLE_H_
