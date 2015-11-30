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
#include "Oid.hpp"

namespace agentxcpp
{
    /**
     * \brief This class represents an Integer variable as described in RFC 
     *        2741.
     *
     * The page \ref variables explains how to use variables.
     *
     * \note When an IntegerVariable is used as INDEX within a table, it should 
     *       never hold the value 0. This is because the value is then used as 
     *       part of an OID, where 0 is forbidden (or at least discouraged).
     */
    class IntegerVariable : public AbstractVariable
    {
	protected:
	    /**
             * \brief The current value.
	     *
             * This is the value which is sent to the master agent on SNMP Get 
             * requests. According to RFC 2578, INTEGER is a signed 32-bit 
             * number. The value can be obtained with value() and set with 
             * setValue().  
             *
             * The perform_get() method should update this member.
             *
             * The methods perform_testset(), perform_commitset(), 
             * perform_cleanupset() and/or perform_undoset() may update this 
             * member, but it is not needed for proper function.
             */
	    qint32 v;

        private:

            /**
             * \brief The new value for the variable. Used during a Set 
             *        operation.
             *
             * The Set operation is performed in up to four steps (TestSet,              
             * CommitSet, CleanupSet, UndoSet). Only the TestSet step 
             * actually
             * receives the new value. This value is stored here so that it can
             * be delivered to perform_commitset(), perform_undoset() and perform_cleanupset().
             */
            QSharedPointer<IntegerVariable> new_value;

	public:

	    /**
             * \brief Constructor.
             *
             * Creates an IntegerValue object.
	     *
	     * \exception None.
	     */
	    IntegerVariable(qint32 _value=0) :v(_value) {}

            /**
	     * \internal
	     *
             * \brief Encode the object as described in RFC 2741, section   
             *        5.4.
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
             *            The iterator is advanced while parsing.
	     *
	     * \param end Iterator pointing one element past the end of the
             *            current stream. This is needed to avoid buffer 
             *            overrun.
	     *
	     * \param big_endian Whether the input stream is in big endian
             *                   format.
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
	     * A negative value with be converted to a big unsigned
	     * subid.
             *
             * This method should not be overridden.
	     *
	     * \note If an INTEGER is used in an INDEX clause, the
	     *       value 0 should be avoided according to
	     *       RFC 2578, 7.7. "Mapping of the INDEX clause".
	     */
	    virtual Oid toOid() const
	    {
	        Oid oid;
	        oid.push_back(v);
	        return oid;
	    }

            /**
             * \brief Set the value.
             * 
             * \param _value The new value.
             */
            void setValue(qint32 _value)
            {
                v = _value;
            }

            /**
             * \brief Get the current value.
             *
             * \return The value.
             */
            virtual qint32 value()
            {
                return v;
            }

            /**
             * \internal
             *
             * \brief Handle a Get Request.
             *
             * This function calls perform_get() to update the internal 
             * value.
             */
            virtual void handle_get()
            {
                perform_get();
            }

            /**
             * \brief Perform a Get request.
             *
             * This method is invoked when an SNMP Get request is received.
             * It should update the internal value \ref v.
             */
            virtual void perform_get()
            {
            }

            /**
             * \internal
             *
             * \brief Handle a TestSet request.
             *
             * This function converts the argument, stores it in \ref new_value 
             * and then calls perform_testset() with the internal value of \ref 
             * new_value.  If conversion fails, 
             * \agentxcpp{AbstractVariable::wrongType} is returned and 
             * perform_testset() is not called.
             *
             * \param _v The new value for the variable.
             *
             * \return \agentxcpp{AbstractVariable::wrongType}
             *         if the conversion fails.  Otherwise, the result of
             *         perform_testset() is returned.
             */
            virtual testset_result_t handle_testset(QSharedPointer<AbstractVariable> _v)
            {
                new_value = qSharedPointerDynamicCast<IntegerVariable>(_v);
                if (new_value)
                {
                    // Type matches variable
                    return perform_testset(new_value->value());
                }
                else
                {
                    // Wrong type
                    return wrongType;
                }

            }

            /**
             * \brief Perform an SNMP TestSet request.
             *
             * This method is invoked when an SNMP TestSet request is 
             * received.
             * It shall check whether a Set operation is possible for the
             * variable.  It shall acquire the resources needed to perform the
             * Set operation (but the Set operation shall not yet be 
             * performed).
             *
             * The default implementation returns 
             * \agentxcpp{AbstractVariable::noAccess} to indicate that
             * this is a read-only variable. Thus, for read-only variables this
             * method need not be overridden.
             *
             * \param _v The new value provided by the master agent.
             *
             * \return The result of the check (this is reported to the master 
             *         agent).
             */
            virtual testset_result_t perform_testset(qint32 _v)
            {
                return noAccess;
            }

            /**
             * \internal
             *
             * \brief Handle a CleanupSet request.
             *
             * This function calls perform_cleanupset() with the internal value 
             * of \ref new_value (which was updated by the last
             * \ref handle_testset() invocation).
             */
            virtual void handle_cleanupset()
            {
                perform_cleanupset(new_value->value());
            }

            /**
             * \brief Perform an SNMP CleanupSet request.
             *
             * This method is invoked when an SNMP CleanupSet request is
             * received. It shall release any resources allocated by 
             * perform_testset().
             *
             * The default implementation does nothing. If no action is
             * required to perform the CleanupSet operation, this method need
             * not be overridden.
             *
             * \param _v The value to which the variable was (possibly) set.
             */
            virtual void perform_cleanupset(qint32 _v)
            {
                return;
            }

            /**
             * \internal
             *
             * \brief Handle a CommitSet request.
             *
             * This function calls \ref perform_commitset() with the internal 
             * value of \ref new_value (which was updated by the last
             * \ref handle_testset() invocation).
             *
             * \return The return value of perform_commitset().
             */
            virtual bool handle_commitset()
            {
                return perform_commitset(new_value->value());
            }

            /**
             * \brief Perform an SNMP CommitSet request.
             *
             * This method is invoked when an SNMP CommitSet request is
             * received. It shall perform the actual write operation.
             *
             * The default implementation returns false to indicate that the
             * operation failed. To implement a writeable SNMP variable this
             * method must be overridden.
             *
             * \param _v The value which shall be written.
             *
             * \return True if the operation succeeded, false otherwise.
             */
            virtual bool perform_commitset(qint32 _v)
            {
                return false;
            }

            /**
             * \internal
             *
             * \brief Handle an UndoSet request.
             *
             * This function calls perform_undoset() with the internal value of 
             * \ref new_value (which was updated by the last
             * \ref handle_testset() invocation).
             *
             * \return The return value of perform_undoset().
             *
             */
            virtual bool handle_undoset()
            {
                return perform_undoset(new_value->value());
            }

            /**
             * \brief Perform an SNMP UndoSet request.
             *
             * This method is invoked when an SNMP UndoSet request is 
             * received.
             * It shall undo whatever perform_commitset() performed. It shall also 
             * release all resources allocated by perform_testset(), because 
             * perform_cleanupset() \e will \e not be called afterwards.
             *
             * The default implementation returns false to indicate that
             * the operation failed. It is strongly recommended that writeable
             * variables override this method.
             *
             * \internal
             *
             * A default implementation is provided to allow implementing
             * read-only variables.
             *
             * \endinternal
             *
             * \param _v The value just set by perform_commitset().
             *
             * \return True on success, false otherwise.
             */
            virtual bool perform_undoset(qint32 _v)
            {
                return false;
            }
    };
}

#endif  // _INTEGERVARIABLE_H_
