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

#ifndef _INTEGERVARIABLE_H_
#define _INTEGERVARIABLE_H_

#include <QtGlobal>

#include "AbstractVariable.hpp"
#include "exceptions.hpp"
#include "OidVariable.hpp"

namespace agentxcpp
{
    /**
     * \brief Represents an Integer as described in RFC 2741
     */
    class IntegerVariable : public AbstractVariable
    {
	protected:
	    /**
	     * \brief The Integer value.
	     *
	     * According to RFC 2578, INTEGER is a signed 32-bit number.
	     */
	    qint32 v;

        private:

            /**
             * \brief The new value for the variable in a Set operation.
             *
             * The Set operation is performed in up to four steps (testset,
             * commitset, cleanupset, undoset). Only the testset step actually
             * receives the new value. This value is stored here so that it can
             * be delivered to commitset(), undoset() and cleanupset().
             */
            QSharedPointer<IntegerVariable> new_value;

	public:

	    /**
             * \internal
             *
	     * \brief Create an IntegerValue object.
	     *
	     * The default value of the new object is 0.
	     *
	     * \exception None.
	     */
	    IntegerVariable(qint32 _value=0) :v(_value) {}

            /**
	     * \internal
	     *
	     * \brief Encode the object as described in RFC 2741, section 5.4
	     *
	     * This function uses big endian.
	     */
	    virtual binary serialize() const;
	    
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
	     * fails, parse_error is thrown. In this case, the iterator 
	     * position is undefined.
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
	     */
	    IntegerVariable(binary::const_iterator& pos,
		    const binary::const_iterator& end,
		    bool big_endian=true);

	    /**
	     * \brief Convert the value to an OID.
	     *
	     * The conversion is done according to RFC 2578,
	     * 7.7. "Mapping of the INDEX clause". The value is
	     * converted to an Oid with a single subid. Note that
	     * INTEGER values are signed, while subids are not.
	     * A negative value with be converted to big unsigned
	     * subid.
	     *
	     * \note If an INTEGER is used in an INDEX clause, the
	     *       value 0 should be avoided according to
	     *       RFC 2578, 7.7. "Mapping of the INDEX clause".
	     */
	    OidVariable toOid() const
	    {
	        OidVariable oid;
	        oid.push_back(v);
	        return oid;
	    }

            void setValue(qint32 _value)
            {
                v = _value;
            }

            qint32 value()
            {
                return v;
            }

            /**
             * \internal
             *
             * \brief Handle a Get Request.
             *
             * This function calls get() to obtain the new value,
             * converts it to QSharedPointer<AbstractValue> and returns it.
             */
            virtual void handle_get()
            {
                this->get();
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
                new_value = qSharedPointerDynamicCast<IntegerVariable>(_v);
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
            virtual testset_result_t testset(QSharedPointer<IntegerVariable> _v)
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
            virtual void cleanupset(QSharedPointer<IntegerVariable> _v)
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
            virtual bool commitset(QSharedPointer<IntegerVariable> _v)
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
            virtual bool undoset(QSharedPointer<IntegerVariable> _v)
            {
                return false;
            }


    };
}

#endif  // _INTEGERVARIABLE_H_
