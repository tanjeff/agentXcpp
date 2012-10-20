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
#ifndef _PRIMITIVE_VARIABLE_H_
#define _PRIMITIVE_VARIABLE_H_

#include <boost/shared_ptr.hpp>

#include "variable.hpp"

namespace agentxcpp
{
    template< class V >
    class primitive_variable : public variable
    {
        private:
            /**
             * \brief The new value for the variable in a Set operation.
             *
             * The Set operation is performed in up to four steps (testset,
             * commitset, cleanupset, undoset). Only the testset step actually
             * receives the new value; the variable must store the value
             * internally for the later steps.
             *
             * This member is the place where the value is stored until the
             * Set operation finishes.
             */
            shared_ptr<V> new_value;

        public:

            virtual shared_ptr<value> internal_get()
            {
                shared_ptr<V> retval( new V(this->get()) );
                return retval;
            }


            virtual V get()
            {
                throw( generic_error() );
            }

            virtual testset_result_t testset(shared_ptr<value> v)
            {
                new_value = boost::dynamic_pointer_cast<V>(v);
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

            virtual testset_result_t testset(shared_ptr<V> v)
            {
                return noAccess;
            }

            virtual void cleanupset()
            {
                cleanupset(new_value);
            }

            virtual void cleanupset(shared_ptr<V> v)
            {
                return;
            }

            virtual bool commitset()
            {
                return commitset(new_value);
            }

            virtual bool commitset(shared_ptr<V> v)
            {
                return false;
            }

    };

}



#endif /* _PRIMITIVE_VARIABLE_H_ */
