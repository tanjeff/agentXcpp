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
#ifndef _OPAQUEVARIABLE_H_
#define _OPAQUEVARIABLE_H_

#include "AbstractVariable.hpp"
#include "exceptions.hpp"

namespace agentxcpp
{
    /**
     * \brief Represents an Opaque object as described in RFC 2741, section
     * 5.4.
     *
     * This class represents binary data. According to RFC 2578,
     * OPAQUE is used to wrap any ASN.1-type.
     */
    class OpaqueVariable : public AbstractVariable
    {
        protected:
            /**
             * \brief The value.
             */
            binary v;

        private:

            /**
             * \copydoc agentxcpp::IntegerVariable::new_value
             */
            QSharedPointer<OpaqueVariable> new_value;

        public:

            /**
             * \internal
             *
             * \brief Encode the object as described in RFC 2741, section 5.4
             *
             * This function uses big endian.
             */
            binary serialize() const;

            /**
             * \internal
             *
             * \copydoc agentxcpp::IntegerVariable::IntegerVariable(binary::const_iterator&, const binary::const_iterator&, bool)
             *
             */
            OpaqueVariable(binary::const_iterator& pos,
                           const binary::const_iterator& end,
                           bool big_endian=true);

            /**
             * \brief Constructor for initializing with data.
             */
            OpaqueVariable(binary initial_value) : v(initial_value) {}

            /**
             * \brief Default constructor.
             *
             * Initialize the object so it holds no data.
             */
            OpaqueVariable()
            {
            }

            /**
             * \copydoc agentxcpp::IntegerVariable::setValue()
             */
            void setValue(binary _value)
            {
                v = _value;
            }

            /**
             * \brief Get the current value.
             *
             * \return The value.
             */
            binary value()
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
                new_value = qSharedPointerDynamicCast<OpaqueVariable>(_v);
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
             * \copydoc agentxcpp::IntegerVariable::perform_testset()
             */
            virtual testset_result_t perform_testset(const binary& _v)
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
            virtual void perform_cleanupset(const binary& _v)
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
            virtual bool perform_commitset(const binary& _v)
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
            virtual bool perform_undoset(const binary& _v)
            {
                return false;
            }

            /**
             * \brief Convert the value to an OID.
             *
             * The conversion is done according to RFC 2578,
             * 7.7. "Mapping of the INDEX clause". First, the
             * length of the data (in bytes) is converted to a subid. Then, each
             * octet of the value is converted into a separate subid.
             *
             * \todo Support fixed-length strings according to RFC 2578,
             *       7.7. "Mapping of the INDEX clause".
             */
            virtual Oid toOid() const;
    };
}

#endif  // _OPAQUEVARIABLE_H_
