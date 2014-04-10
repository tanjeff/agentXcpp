/*
 * Copyright 2011-2013 Tanjeff-Nicolai Moos <tanjeff@cccmz.de>
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

#ifndef _TABLEENTRY_HPP_
#define _TABLEENTRY_HPP_

#include "Oid.hpp"
#include "AbstractVariable.hpp"

namespace agentxcpp
{

    /**
     * \brief Base class for table entries.
     */
    class TableEntry
    {
        public:

            /**
             * \brief Get the index for this entry.
             *
             * This method shall provide the index for the entry. The index is 
             * the concatenation of the index variables, converted to OID's.  
             * See ///TUTORIAL/// for an example.
             *
             * \return The index of the entry.
             *
             * \exception This method shall not throw.
             */
            virtual Oid index() = 0;

            /**
             * \brief Get the variable with of column 'id'.
             *
             * This method shall return the variable with the column 'id'.  If 
             * no variable is available for the column, the NULL pointer shall 
             * be returned.
             * 
             * The variables of the table entry shall be consequtive, to 
             * support iterating the entry (i.e. by incrementing 'id' until 
             * the NULL pointer is returned).
             *
             * \return The variable with column 'id'.
             *
             * \exception This method shall not throw.
             */
            virtual QSharedPointer<AbstractVariable> getVariable(quint32 id) = 0;

            /**
             * \brief Virtual Destructor.
             */
            virtual ~TableEntry()
            {
            }

        private:
    };

} /* namespace agentxcpp */
#endif /* _TABLEENTRY_HPP_ */
