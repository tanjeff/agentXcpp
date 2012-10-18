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
#ifndef _VARIABLE_H_
#define _VARIABLE_H_

#include "binary.hpp"

namespace agentxcpp
{
    /**
     * \brief This class represents a SNMP variable.
     *
     * This class is used as base class for the SNMP variable types (such as 
     * Octet_String or Integer). It has no functions or members exposed to the 
     * API.
     */
    class variable
    {
	public:
	    /**
             * \internal
             *
	     * \brief Serialize the variable.
	     *
	     * This function must be implemented by all derived classes. The 
	     * function shall generate a serialized form of the variable.
	     *
	     * \return The serialized form of the variable.
	     *
	     * \exception None: The function shall not throw.
	     */
	    // Needed for encoding varbinds
	    virtual binary serialize() const = 0;

            /**
             * \brief Destructor.
             */
             virtual ~variable()
             {
             }

            /**
             * \internal
             *
             * \brief Update the internal state of the variable.
             *
             * This function must be implemented in derived classes. It shall 
             * update the internal state of the object.
             *
             * \exception generic_error If obtaining the new value failed. The
             *                          state of such an object cannot be 
             *                          updated.
             */
            virtual void update()=0;

            /**
             * \brief Result type for the TestSet validation.
             *
             * A value of this type is returned during a TestSet operation.
             *
             * Note that the result 'wrongEncoding' is not used within the 
             * agentXcpp implementation and is therefore not part of this 
             * enumeration.
             *
             * \internal
             *
             * The numeric values are given in RFC 2741, 7.2.4.1. "Subagent 
             * Processing of the agentx-TestSet-PDU" and are in sync with the 
             * corresponding errors defined in ResponsePDU::error_t.
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
             * \brief Validate whether a set operation would be successful.
             *
             * This function implements the validation step performed during a 
             * TestSet operation. This step checks whether a real Set operation 
             * is possible for the variable and acquires the resources needed 
             * to perform a Set operation.
             *
             * \return The result of the validation. See testset_result_t for
             *         the possible values.
             */
            virtual testset_result_t testset()=0;

            /**
             * \brief Release resources after a Set operation.
             *
             * This function is called after a Set operation. The variable 
             * should release all resources which were allocated during the 
             * testset() function.
             *
             * The default behaviour is to do nothing, thus a variable need not 
             * to override this method if it doesn't need to release resources.
             */
            virtual void cleanupset()
            {
                // do nothing
            }

            /**
             * \brief Actually perform the set operation.
             *
             * This function implements the set operation. It must report 
             * whether the operation succeeded.
             *
             * \return True on success, false otherwise.
             */
            virtual bool commitset()
            {
                return false;
            }
    };
}


#endif
