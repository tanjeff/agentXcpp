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
             * \brief Get the index variables of this entry.
             *
             * This method shall provide the index variables for the entry.
             * The order in the vector shall match the order specified
             * in the MIB.
             *
             * \return The index variables of the entry.
             *
             * \exception This method shall not throw.
             */
            virtual QVector< QSharedPointer<AbstractVariable> > indexVariables() = 0;

            /**
             * \brief Get the variables of this entry.
             *
             * This method shall return the variables for this entry. It is
             * assumed that the index of a variable in the returned vector
             * matches its index specified in the MIB.
             * 
             * \return The variables for this entry.
             *
             * \exception This method shall not throw.
             */
            virtual QVector< QSharedPointer<AbstractVariable> > variables() = 0;

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
