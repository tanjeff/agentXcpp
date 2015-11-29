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

#ifndef _OIDVARIABLE_H_
#define _OIDVARIABLE_H_

#include <QtGlobal>

#include "AbstractVariable.hpp"
#include "Oid.hpp"
#include "exceptions.hpp"


namespace agentxcpp
{
    /**
     * \brief Represents an SNMP OBJECT-IDENTENTIFIER variable.
     *
     * The agentXcpp library distinguishes between Oid and
     * OidVariable types. An Oid is a sequence of numbers,
     * while OidVariable is an SNMP variable representing an Oid. That
     * means, that Get and Set requests are possible on OidVariable's, but
     * not on Oid's.
     */
    class OidVariable: public AbstractVariable
    {
        protected:
            /**
             * \brief The value represented by this variable.
             */
            Oid v;

        private:

            /**
             * \copydoc agentxcpp::IntegerVariable::new_value
             */
            QSharedPointer<OidVariable> new_value;

        public:

            /**
             * \brief Initialize an OidVariable object with an Oid.
             *
             * The internal value is set to the provided Oid.
             *
             * \param o The initial Oid.
             *
             * \exception None.
             */
            OidVariable(const Oid& o);

            /**
             * \brief Default constructor.
             *
             * Initialize the value to the null Oid.
             */
            OidVariable()
            {
            }

            /**
             * \brief Get the current value.
             *
             * \return The value.
             */
            Oid value()
            {
                return v;
            }

            /**
             * \copydoc agentxcpp::IntegerVariable::setValue()
             */
            void setValue(const Oid& _value)
            {
                v = _value;
            }

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
            OidVariable(binary::const_iterator& pos,
                        const binary::const_iterator& end,
                        bool big_endian=true);

            /**
             * \brief Convert the value to an OID.
             *
             * The conversion is done according to RFC 2578,
             * 7.7. "Mapping of the INDEX clause". First, the
             * length of the OID (i.e. the number of subid's it has) is
             * converted to a subid. Then, each subid of the value is appended.
             *
             * \todo Support fixed-length OIDs according to RFC 2578,
             *       7.7. "Mapping of the INDEX clause".
             *
             * \return The generated Oid.
             *
             * \exception None.
             */
            virtual Oid toOid() const
            {
                // fixed length currently unsupported:
                bool fixedLength = false;

                Oid oid;

                // Store length if needed
                if(!fixedLength)
                {
                    oid.push_back(v.size());
                }

                // Store value
                oid += v;

                return oid;
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
                new_value = qSharedPointerDynamicCast<OidVariable>(_v);
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
            virtual testset_result_t perform_testset(const Oid& _v)
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
            virtual void perform_cleanupset(const Oid& _v)
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
            virtual bool perform_commitset(const Oid& _v)
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
            virtual bool perform_undoset(const Oid& _v)
            {
                return false;
            }
    };
}


#endif  // _OIDVARIABLE_H_
