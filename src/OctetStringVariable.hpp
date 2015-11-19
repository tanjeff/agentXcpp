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
             * \brief Construct object from binary data.
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
            OctetStringVariable(binary::const_iterator& pos,
                                const binary::const_iterator& end,
                                bool big_endian=true);

            /**
             * \brief Set the current value.
             */
            void setValue(binary new_value)
            {
                v = new_value;
            }

            /**
             * \brief Set the current value.
             */
            void setValue(QString new_value);

            /**
             * \brief Get the current value
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
             * \brief Handle a Get Request.
             */
            virtual QSharedPointer<AbstractVariable> handle_get()
            {
                this->get();
                return QSharedPointer<OctetStringVariable>(new OctetStringVariable(*this));
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
             * \param _v The new value for the variable.
             *
             * \return agentxcpp::AbstractVariable::wrongType if the conversion
             *                                                fails. Otherwise,
             *                                                the result of
             *                                                testset() is
             *                                                returned.
             */
            virtual testset_result_t handle_testset(QSharedPointer<AbstractVariable> _v)
            {
                new_value = qSharedPointerDynamicCast<OctetStringVariable>(_v);
                if (new_value)
                {
                    // Type matches variable
                    return testset(new_value->value());
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
             * \param _v The new value for the object.
             *
             * \return The result of the check.
             */
            virtual testset_result_t testset(const binary& _v)
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
                cleanupset(new_value->value());
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
             * \param _v The new value for the object.
             */
            virtual void cleanupset(const binary& _v)
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
                return commitset(new_value->value());
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
             * \param _v The new value for the object.
             *
             * \return True if the operation succeeded, false otherwise.
             */
            virtual bool commitset(const binary& _v)
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
                return undoset(new_value->value());
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
             * \param _v The new value for the object.
             *
             * \return True on success, false otherwise.
             */
            virtual bool undoset(const binary& _v)
            {
                return false;
            }
    };
}
#endif // _OCTETSTRINGVARIABLE_H_
