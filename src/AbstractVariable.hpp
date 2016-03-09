/*
 * Copyright 2011-2016 Tanjeff-Nicolai Moos <tanjeff@cccmz.de>
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
#ifndef _ABSTRACTVARIABLE_H_
#define _ABSTRACTVARIABLE_H_

#include <QSharedPointer>

#include "binary.hpp"
#include "Oid.hpp"

namespace agentxcpp
{
    /**
     * \brief Base class for SNMP variables.
     *
     * This class is the base class for SNMP variable implementations. It 
     * provides the interface which is used by agentXcpp (namely by the 
     * MasterProxy class) to perform operations on variables. An overiew on 
     * variable handling in agentxcpp is given in \ref variables.
     *
     * \note This class should never be inherited by non-agentXcpp code.
     *
     */
    class AbstractVariable
    {

        public:

            /**
             * \brief Virtual destructor.
             */
            virtual ~AbstractVariable()
            {
            }

            /**
             * \internal
             *
             * \brief Handle AgentX Get request.
             *
             * This method is called by the MasterProxy object when the SNMP 
             * request "Get" is received for the variable. It should update the 
             * internal state.
             *
             * \note Any exception thrown by this method results in sending a 
             *       generic error to the master agent.
             *
             * \exception generic_error If obtaining the current value fails.
             *                          No other exception shall be thrown.
             */
            virtual void handle_get() = 0;


            /**
             * \brief Result type for the TestSet validation.
             *
             * A value of this type is returned during a TestSet operation 
             * according to  RFC 2741, 7.2.4.1. "Subagent Processing of the 
             * agentx-TestSet-PDU".
             *
             * \note The value 'wrongEncoding' is not used within the
             *       agentXcpp implementation and is therefore not part of this 
             *       enumeration.
             *
             * The numeric values are given in RFC 2741, 7.2.4.1. "Subagent 
             * Processing of the agentx-TestSet-PDU".
             *
             * \internal
             *
             * \note These values must be in sync with the corresponding
             *       errors defined in agentxcpp::ResponsePDU::error_t.
             *
             */
            enum testset_result_t
            {
                /**
                 * Validation succeeded.
                 */
                noError = 0,

                /**
                 * The variable could not be set, and there is no other
                 * enumeration value to express the reason.
                 */
                genErr = 5,

                /**
                 * Access denied (i.e. the object is read-only).
                 */
                noAccess = 6,

                /**
                 * The object cannot be set to the requested type because it
                 * has the wrong type (e.g. an OctetString variable cannot be
                 * set to an Integer value).
                 */
                wrongType = 7,

                /**
                 * The requested new value has the wrong length. E.g. an
                 * OctetString variable may be restricted to 8 characters, but
                 * the requested new string has 15 characters.
                 */
                wrongLength = 8,

                /**
                 * The requested value could under no circumstances be assigned
                 * to the variable (although the variable is read-write). For
                 * example, certain values may be disallowed for a specific
                 * Integer variable.
                 */
                wrongValue = 10,

                /**
                 * The variable does not exist and could not ever be created.
                 */
                noCreation = 11,

                /**
                 * The requested value could under other circumstances be held
                 * by the variable, but is presently inconsistent or otherwise
                 * unable to be assigned to the variable.
                 */
                inconsistentValue = 12,

                /**
                 * A resource which is required to perform the actual Set
                 * operation is unavailable. A Set operation would fail because
                 * of this reason.
                 */
                resourceUnavailable = 13,

                /**
                 * The object doesn't exist and cannot be created on-the-fly,
                 * or the variable exists but cannot be modified.
                 *
                 * \note Use noAccess for read-only variables.
                 */
                notWritable = 17,

                /**
                 * The variable does not exist and can not be created under the
                 * present circumstances (even though it could be created under
                 * other circumstances).
                 */
                inconsistentName = 18
            };

            /**
             * \internal
             *
             * \brief Validate whether a Set operation would be successful.
             *
             * This method is called when the SNMP request "TestSet" is
             * received. It shall check whether a Set operation is possible for 
             * the variable. It shall acquire the resources needed to perform 
             * the Set operation (but the Set shall not yet be performed).
             *
             * \note This is the only method which receives the new value to be
             *       set. An implementation must save the new value for 
             *       subsequent operations (i.e.  handle_commitset()).
             *
             * \return The result of the validation.
             *
             * \exception The function shall not throw.
             */
            virtual testset_result_t handle_testset(QSharedPointer<AbstractVariable>) = 0;

            /**
             * \internal
             *
             * \brief Release resources after a Set operation.
             *
             * This method is called when the SNMP request "CleanupSet"
             * is received. It shall release all resources previously allocated
             * by handle_testset() (if any). If no resources were allocated,
             * this method is not required to do anything.
             *
             * \exception The function shall not throw.
             */
            virtual void handle_cleanupset() = 0;

            /**
             * \internal
             *
             * \brief Actually perform the Set operation.
             *
             * This method is called when the SNMP request "CommitSet"
             * is received for the variable. It shall perform the Set 
             * operation. It shall report whether the operation succeeded.
             *
             * \note The new value is given to handle_testset() prior to
             *       calling handle_commitset().
             *
             * \return True on success, false otherwise.
             *
             * \exception The function shall not throw.
             */
            virtual bool handle_commitset()= 0;

            /**
             * \internal
             *
             * \brief Undo a Set operation which was already performed.
             *
             * This method is called when the SNMP request "UndoSet" is
             * received. It shall undo the operation performed by 
             * handle_commitset().
             *
             * \return True on success, false otherwise.
             *
             * \exception The function shall not throw.
             */
            virtual bool handle_undoset() = 0;

            /**
             * \internal
             *
             * \brief Serialize the variable.
             *
             * This function shall generate a serialized form of the internal 
             * variable (i.e. the network representation of the variable).
             *
             * \return The serialized form of the variable.
             *
             * \exception The function shall not throw.
             */
            virtual binary serialize() const = 0;

            /**
             * \brief Convert an INDEX variable to an Oid part.
             *
             * If an SNMP variable is used as INDEX within a table,
             * then its value is used as part of the Oid for that table
             * entry. Therefore, such variables must be convertible to Oid's.  
             * This method provides this conversion.
             *
             * Not all variable types are allowed to be used as INDEX and are
             * therefore not convertible to Oid. For variables which are not
             * convertible, this method shall return the null Oid.
             *
             * \return The Oid representing the variables value, or the null 
             *         Oid if the variable don't support such conversion.
             *
             * \exception This method shall not throw.
             */
            virtual Oid toOid() const = 0;
    };
}



#endif /* _ABSTRACTVARIABLE_H_ */
