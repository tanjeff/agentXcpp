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

#include <QMap>
#include <QVector>

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
             * \brief The subid assigned to this entry.
             *
             * This variable shall be initialized by a subclass before
             * the entry is added to a table. See \ref how_tables_work for
             * an introduction to this value.
             */
            quint32 subid;

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
             * This method shall return the variables for this entry. The
             * variables are returned as a map, with the variables index
             * as key and the variable itself as value.
             * 
             * \return The variables for this entry.
             *
             * \exception This method shall not throw.
             */
            virtual QMap< quint32, QSharedPointer<AbstractVariable> > variables() = 0;

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
