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

#ifndef _IPADDRESSVARIABLE_H_
#define _IPADDRESSVARIABLE_H_

#include <QtGlobal>

#include "AbstractVariable.hpp"
#include "exceptions.hpp"
#include "Oid.hpp"

namespace agentxcpp
{
    /**
     * \brief Represents an IP address as described in RFC 2741, section 5.4
     */
    class IpAddressVariable : public AbstractVariable
    {
        protected:
	    /**
	     * \brief The IP address.
	     *
	     * According to RFC 2578, IpAddress is a 32-bit number.
	     */
            quint8 v[4]; // only IPv4

	private:
	    /**
	     * \brief Hidden default constructor.
             *
             * This constructor is private so it cannot be used to construct
             * objects.
	     */
            IpAddressVariable();

            /**
             * \copydoc agentxcpp::IntegerVariable::new_value
             */
            QSharedPointer<IpAddressVariable> new_value;

	public:
            /**
             * \internal
             *
             * \copydoc agentxcpp::IntegerVariable::IntegerVariable(binary::const_iterator&, const binary::const_iterator&, bool)
             *
             */
            IpAddressVariable(binary::const_iterator& pos,
		      const binary::const_iterator& end,
		      bool big_endian=true);

	    /**
	     * \internal
	     *
	     * \brief Encode the object as described in RFC 2741, section 5.4 / 
	     * 5.3
	     *
	     * Note:
	     * We always use big endian.
	     */
	    binary serialize() const;

	    /**
             * \brief Construct an IpAddressValue object.
             *
             * The stored IpAddress is initialized to "a.b.c.d".
             *
             * \exception None.
	     */
	    IpAddressVariable(quint8 a,
		      quint8 b,
		      quint8 c,
		      quint8 d)
	    {
		v[0] = a;
		v[1] = b;
		v[2] = c;
		v[3] = d;
	    }


            /**
             * \brief Set the value.
             *
             * The stored IpAddress is updated to "a.b.c.d".
             *
             * \exception None.
             */
	    void set_value(quint8 a,
                           quint8 b,
                           quint8 c,
                           quint8 d)
            {
                v[0] = a;
                v[1] = b;
                v[2] = c;
                v[3] = d;
            }

	    /**
	     * \brief Access a component of the stored IpAddress.
	     *
	     * This function returns the component with the given index. Note
	     * that exactly 4 components are stored. The return value is a
	     * reference to the component, so that it can be read/written, in
	     * other words, the IpAddressValue object can be accessed like an
	     * ordinary array.
	     *
	     * \param index The index (value in the range 0...3).
	     *
	     * \return A reference to the component.
	     *
	     * \exception inval_param If the index is out of bound.
	     */
	    quint8& operator[](unsigned index)
	    {
	        if(index > 3)
	        {
	            throw(inval_param());
	        }
	        return v[index];
	    }

	    /**
	     * \brief Convert the value to an OID.
	     *
	     * The conversion is done according to RFC 2578,
	     * 7.7. "Mapping of the INDEX clause". The created OID
	     * has exactly 4 subids which corresponds to the 4
	     * integers of the IP address.
	     */
	    virtual Oid toOid() const
	    {
	        Oid oid;
	        oid.push_back(v[0]);
	        oid.push_back(v[1]);
	        oid.push_back(v[2]);
	        oid.push_back(v[3]);
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
                new_value = qSharedPointerDynamicCast<IpAddressVariable>(_v);
                if (new_value)
                {
                    // Type matches variable
                    return perform_testset(new_value->v);
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
            virtual testset_result_t perform_testset(const quint8 _v[4])
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
                perform_cleanupset(new_value->v);
            }

            /**
             * \copydoc agentxcpp::IntegerVariable::perform_cleanupset()
             */
            virtual void perform_cleanupset(const quint8 _v[4])
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
                return perform_commitset(new_value->v);
            }

            /**
             * \copydoc agentxcpp::IntegerVariable::perform_commitset()
             */
            virtual bool perform_commitset(const quint8 _v[4])
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
                return perform_undoset(new_value->v);
            }

            /**
             * \copydoc agentxcpp::IntegerVariable::perform_undoset()
             */
            virtual bool perform_undoset(const quint8 _v[4])
            {
                return false;
            }

    };
}

#endif  // _IPADDRESSVARIABLE_H_
