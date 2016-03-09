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

#ifndef _GAUGE32VARIABLE_H_
#define _GAUGE32VARIABLE_H_

#include <QtGlobal>

#include "AbstractVariable.hpp"
#include "exceptions.hpp"

namespace agentxcpp
{
    /**
     * \brief Represents a Gauge32 as described in RFC 2741
     */
    class Gauge32Variable : public AbstractVariable
    {
	protected:
	    /**
	     * \brief The value.
	     *
	     * According to RFC 2578, Gauge32 is a non-negative 32-bit 
	     * number.
	     */
	    quint32 v;

	private:

            /**
             * \copydoc agentxcpp::IntegerVariable::new_value
             */
            QSharedPointer<Gauge32Variable> new_value;


	public:

	    /**
             * \brief Default Constructor.
	     *
	     * The value after creation is 0.
	     */
	    Gauge32Variable()
   	    : v(0)
	    {
	    }

	    /**
	     * \internal
             *
             * \copydoc agentxcpp::IntegerVariable::IntegerVariable(binary::const_iterator&, const binary::const_iterator&, bool)
	     *
	     */
	    Gauge32Variable(binary::const_iterator& pos,
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

            /**
             * \brief Get the current value.
             *
             * \return The value.
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
             * \copydoc agentxcpp::IntegerVariable::handle_testset()
             */
            virtual testset_result_t handle_testset(QSharedPointer<AbstractVariable> _v)
            {
                new_value = qSharedPointerDynamicCast<Gauge32Variable>(_v);
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
             * \copydoc agentxcpp::IntegerVariable::perform_testset()
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
             */
            virtual bool perform_commitset(quint32 _v)
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
             * \brief Convert the value to an OID.
	     *
	     * The conversion is done according to RFC 2578,
	     * 7.7. "Mapping of the INDEX clause". The value is
             * converted to an Oid with a single subid.
             *
             * This method should not be overridden.
	     *
             * \note If an GAUGE32 is used in an INDEX clause, the
	     *       value 0 should be avoided according to
	     *       RFC 2578, 7.7. "Mapping of the INDEX clause".
	     */
            virtual Oid toOid() const
            {
                Oid oid;
                oid.push_back(v);
                return oid;
            }


    };
}

#endif  // _GAUGE32VARIABLE_H_
