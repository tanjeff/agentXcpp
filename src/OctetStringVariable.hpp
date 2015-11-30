#ifndef _OCTETSTRINGVARIABLE_H_
#define _OCTETSTRINGVARIABLE_H_

#include <QString>
#include "AbstractVariable.hpp"
#include "Oid.hpp"

namespace agentxcpp
{
    /**
     * \brief Represents an OctetSting as described in RFC 2741.
     */
    class OctetStringVariable : public AbstractVariable
    {
        protected:

            /**
             * \brief The value.
             *
             * According to RFC 2578, Octet_String represents arbitrary binary
             * or textual data.
             */
            binary v;

        private:

            /**
             * \copydoc agentxcpp::IntegerVariable::new_value
             */
            QSharedPointer<OctetStringVariable> new_value;

        public:

            /**
             * \internal
             *
             * \brief Encode the object as described in RFC 2741, section 5.3
             *
             * \note We always use big endian.
             */
            binary serialize() const;

            /**
             * \brief (Default) constructor.
             *
             * Construct object from binary data, or construct the empty string.
             */
            OctetStringVariable(binary _value = binary())
                : v(_value)
            {
            }

            /**
             * \brief Construct object from a string.
             */
            OctetStringVariable(QString initial_value);

            /**
             * \internal
             *
             * \copydoc agentxcpp::IntegerVariable::IntegerVariable(binary::const_iterator&, const binary::const_iterator&, bool)
             *
             */
            OctetStringVariable(binary::const_iterator& pos,
                                const binary::const_iterator& end,
                                bool big_endian=true);

            /**
             * \copydoc agentxcpp::IntegerVariable::setValue()
             */
            void setValue(binary _value)
            {
                v = _value;
            }

            /**
             * \copydoc agentxcpp::IntegerVariable::setValue()
             */
            void setValue(QString _value);

            /**
             * \brief Get the current value.
             *
             * \return The value.
             */
            binary value() const
            {
                return v;
            }

            /**
             * \brief Get the current value as string.
             */
            QString toString() const;

            /**
             * \brief Convert the value to an OID.
             *
             * The conversion is done according to RFC 2578,
             * 7.7. "Mapping of the INDEX clause". First, the
             * string length is converted to a subid. Then, each
             * octet of the value is converted into a separate subid.
             *
             * \todo Support fixed-length strings according to RFC 2578,
             *       7.7. "Mapping of the INDEX clause".
             */
            virtual Oid toOid() const;

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
                new_value = qSharedPointerDynamicCast<OctetStringVariable>(_v);
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
    };
}
#endif // _OCTETSTRINGVARIABLE_H_
