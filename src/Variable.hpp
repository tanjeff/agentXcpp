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

#include <QSharedPointer>

#include "AbstractVariable.hpp"

namespace agentxcpp
{
    /**
     * \brief Class template to help implement SNMP variables.
     *
     * This class template can be used as base class for SNMP variable
     * implementations. It implements the agentxcpp::AbstractVariable
     * interface and tailors it for a concrete type (in contrast,
     * agentxcpp::AbstractVariable deals with agentxcpp::AbstractValue).
     *
     * It shall be instantiated with the value type the variable
     * provides (which should be a class derived from agentxcpp::AbstractValue).
     * Then, some functions shall be overridden to provide the
     * functionality. Example:
     * 
     * \code
     * // An IntegerValue variable
     * class simpleCounter : public Variable<IntegerValue>
     * {
     *     private:
     *         // The internal counter
     *         IntegerValue counter;
     *
     *     public:
     *
     *         // Handle SNMP get request, overrides Variable::get()
     *         virtual IntegerValue get()
     *         {
     *             return counter;
     *         }
     *
     *         // Increment the internal counter
     *         void increment()
     *         {
     *             counter.value++;
     *         }
     * };
     * \endcode
     *
     * The get() method \e must be implemented. It is not allowed (and not 
     * possible) to implement write-only SNMP variables.
     *
     * To support write access (which is optional), the methods testset() and 
     * commitset() must be implemented. The undoset() method should also be 
     * implemented in this case. The cleanupset() method may be implemented if
     * needed.
     *
     * \internal
     *
     * This class template implements the methods of agentxcpp::AbstractVariable
     * in such a way that they call methods for the more specific value types.
     * Example:
     *
     * \code
     * virtual testset_result_t handle_testset(QSharedPointer<AbstractValue> v)
     *     {
     *         new_value = qSharedPointerDynamicCast<T>(v);
     *         if (new_value)
     *         {
     *             // Type matches variable
     *             return this->testset(new_value);
     *         }
     *         else
     *         {
     *             // Wrong type
     *             return wrongType;
     *         }
     *     }
     * \endcode
     *
     * As can be seen, handle_testset() receives a pointer to an
     * agentxcpp::AbstractValue object.
     * This pointer is casted to 'T' (the template parameter) before giving it
     * to the testset() method.  The testset() method is the one which can be
     * implemented in derived classes (while handle_testset() should not be
     * re-implemented).
     *
     * This class template remembers the new value and gives it to the 
     * functions testset(), commitset(), cleanupset() and undoset(). Also, type 
     * conversion is done (see the example) and an error is generated on type 
     * mismatch.
     */
    template< class T >
    class Variable : public AbstractVariable
    {
        private:
            /**
             * \brief The new value for the variable in a Set operation.
             *
             * The Set operation is performed in up to four steps (testset,
             * commitset, cleanupset, undoset). Only the testset step actually
             * receives the new value. This value is stored here so that it can 
             * be delivered to commitset(), undoset() and cleanupset().
             */
            QSharedPointer<T> new_value;

        public:

            /**
             * \internal
             *
             * \brief Handle a Get Request.
             *
             * This function calls get() to obtain the new value,
             * converts it to QSharedPointer<AbstractValue> and returns it.
             */
            virtual QSharedPointer<AbstractValue> handle_get()
            {
                // Get new value by calling this->get(), then allocate a new 
                // variable, initializing it with the new value:
                QSharedPointer<T> retval( new T(this->get()) );

                return retval;
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
            virtual T get()= 0;

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
            virtual testset_result_t handle_testset(QSharedPointer<AbstractValue> v)
            {
                new_value = qSharedPointerDynamicCast<T>(v);
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
            virtual testset_result_t testset(QSharedPointer<T> v)
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
            virtual void cleanupset(QSharedPointer<T> v)
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
            virtual bool commitset(QSharedPointer<T> v)
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
            virtual bool undoset(QSharedPointer<T> v)
            {
                return false;
            }

    };

}



#endif /* _VARIABLE_H_ */
