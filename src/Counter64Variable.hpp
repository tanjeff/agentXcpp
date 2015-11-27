#ifndef _COUNTER64VARIABLE_H_
#define _COUNTER64VARIABLE_H_

#include "AbstractVariable.hpp"

namespace agentxcpp
{
    /**
     * \brief Represents an Counter64 as described in RFC 2741.
     */
    class Counter64Variable : public AbstractVariable
    {
        protected:

            /**
             * \brief The counter value.
             *
             * According to RFC 2578, Counter64 is a non-negative 64-bit
             * number.
             */
            quint64 v;

        private:

            /**
             * \copydoc agentxcpp::IntegerVariable::new_value
             */
            QSharedPointer<Counter64Variable> new_value;

        public:

            /**
             * \brief Constructor.
             */
            Counter64Variable(quint64 _value = 0)
            : v(_value)
            {
            }

            /**
             * \internal
             *
             * \copydoc agentxcpp::IntegerVariable::IntegerVariable(binary::const_iterator&, const binary::const_iterator&, bool)
             *
             */
            Counter64Variable(binary::const_iterator& pos,
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
            void setValue(quint64 _value)
            {
                v = _value;
            }

            /**
             * \brief Get the current value.
             *
             * \return The value.
             */
            virtual quint64 value()
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
                new_value = qSharedPointerDynamicCast<Counter64Variable>(_v);
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
            virtual testset_result_t perform_testset(quint64 _v)
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
            virtual void perform_cleanupset(quint64 _v)
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
            virtual bool perform_commitset(quint64 _v)
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
            virtual bool perform_undoset(quint64 _v)
            {
                return false;
            }

            /**
             * \brief Return the null Oid.
             *
             * Objects of this class are not allowed in INDEX clauses according
             * to RFC 2578, 7.7. "Mapping of the INDEX clause". Therefore, this
             * method returns the null Oid.
             */
            virtual Oid toOid() const
            {
                return Oid();
            }

    };
}
#endif // _COUNTER64VARIABLE_H_
