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

#include "Table.hpp"

using namespace agentxcpp;

bool Table::addEntry(QSharedPointer<TableEntry> entry)
{
    if(! myMasterProxy)
    {
        return false;
    }

    Oid entryIndex = entry->index();

    // Ensure that index is not registered
    if(entries.contains(entry))
    {
        // The entry was already added
        return false;
    }

    // Register entry
    entries[entry] = entryIndex;

    Oid index = myOid + entryIndex;

    // Register all variables of the entry
    quint32 i = 1;
    QSharedPointer<AbstractVariable> var;
    QVector< QPair< Oid,QSharedPointer<AbstractVariable> > > toRegister;
    while( (var = entry->getVariable(i)) )
    {
        // Add variable to list
        toRegister.append(qMakePair(index + i, var));

        ++i;
    }
    myMasterProxy->addVariables(toRegister);

    // All went well, as far as we can tell.
    return true;
}


bool Table::removeEntry(QSharedPointer<TableEntry> entry)
{
    if(! myMasterProxy)
    {
        return false;
    }

    // Ensure that entry is known
    if(entries.contains(entry))
    {
        // Entry not found
        return false;
    }

    // Unregister all variables of the entry
    Oid entryIndex = entries[entry]; // Use index at time of registration
    quint32 i = 1;
    QSharedPointer<AbstractVariable> var;
    QVector<Oid> toUnregister;
    while( (var = entry->getVariable(i)) )
    {
        // Add variable to list
        toUnregister.append(myOid + entryIndex + i);

        ++i;
    }
    myMasterProxy->removeVariables(toUnregister);

    // Remove entry from local storage
    entries.remove(entry);

    // All went well, as far as we can tell.
    return true;

}
