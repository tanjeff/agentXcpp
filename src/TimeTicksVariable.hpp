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
#ifndef _TIMETICKSVARIABLE_H_
#define _TIMETICKSVARIABLE_H_

#include <QtGlobal>

#include "AbstractVariable.hpp"
#include "exceptions.hpp"


namespace agentxcpp
{
    /**
     * \brief Represents an TimeTicks as described in RFC 2741
     */
    class TimeTicksVariable : public AbstractVariable
    {
	protected:
            /**
	     * \brief The TimeTicks value.
	     *
	     * According to RFC 2578, TimeTicks is a non-negative 32-bit
	     * number.
	     */
	    quint32 v;

	private:

            /**
             * \copydoc agentxcpp::IntegerVariable::new_value
             */
            QSharedPointer<TimeTicksVariable> new_value;

	public:

	    /**
	     * \brief Create an TimeTicksValue without initialization.
	     *
	     * \param initial_value The initial value of the object.
	     */
	    TimeTicksVariable(quint32 initial_value = 0)
	    : v(initial_value)
	    {
	    }

            /**
             * \internal
             *
             * \copydoc 
             * agentxcpp::IntegerVariable::IntegerVariable(binary::const_iterator&, 
             * const binary::const_iterator&, bool)
             *
             */
            TimeTicksVariable(binary::const_iterator& pos,
		      const binary::const_iterator& end,
		      bool big_endian=true);
	    
	    /**
	     * \internal
	     *
	     * \brief Encode the object as described in RFC 2741, section 5.4
	     *
	     * This function uses big endian.
	     */
	    virtual binary serialize() const;

            /**
             * \copydoc agentxcpp::IntegerVariable::setValue()
             */
            void setValue(quint32 _value)
            {
	        v = _value;
	    }

            /*
             * \copydoc agentxcpp::IntegerVariable::value()
             */
            quint32 value()
            {
                return v;
	    }

            /**
             * \internal
             *
             * \copydoc agentxcpp::IntegerVariable::handle_get()
             */
            virtual void handle_get()
            {
                perform_get();
            }

            /**
             * \copydoc agentxcpp::IntegerVariable::perform_get()
             */
            virtual void perform_get()
            {
            }

            /**
             * \internal
             *
             * \copydoc agentxcpp::IntegerVariable::handle_testet()
             */
            virtual testset_result_t handle_testset(QSharedPointer<AbstractVariable> _v)
            {
                new_value = qSharedPointerDynamicCast<TimeTicksVariable>(_v);
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
             * \brief Handle a TestSet request.
             *
             * \copydoc agentxcpp::IntegerVariable::perform_testet()
             */
            virtual testset_result_t perform_testset(quint32 _v)
            {
                return noAccess;
            }

            /**
             * \internal
             *
             * \copydoc agentxcpp::IntegerVariable::handle_cleanupset()
             */
            virtual void handle_cleanupset()
            {
                perform_cleanupset(new_value->value());
            }

            /**
             * \copydoc agentxcpp::IntegerVariable::perform_cleanupset()
             */
            virtual void perform_cleanupset(quint32 _v)
            {
                return;
            }

            /**
             * \internal
             *
             * \copydoc agentxcpp::IntegerVariable::handle_commitset()
             */
            virtual bool handle_commitset()
            {
                return perform_commitset(new_value->value());
            }

            /**
             * \copydoc agentxcpp::IntegerVariable::perform_commitset()
             */ virtual bool perform_commitset(quint32 _v)
            {
                return false;
            }

            /**
             * \internal
             *
             * \copydoc agentxcpp::IntegerVariable::handle_undoset()
             */
            virtual bool handle_undoset()
            {
                return perform_undoset(new_value->value());
            }

            /**
             * \copydoc agentxcpp::IntegerVariable::perform_undoset()
             */
            virtual bool perform_undoset(quint32 _v)
            {
                return false;
            }

            /**
             * \brief Return the null Oid.
             *
             * This method returns the null Oid because RFC 2578, 7.7.
             * "Mapping of the INDEX clause" does not
             * describe how to convert Timeicks to an OID.
             */
            virtual Oid toOid() const
            {
                return Oid();
            }

    };
}

#endif  // _TIMETICKSVARIABLE_H_
