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

#ifndef _TABLE_HPP_
#define _TABLE_HPP_

#include <QMap>
#include <QSharedPointer>

#include "Oid.hpp"
#include "TableEntry.hpp"
#include "MasterProxy.hpp"


namespace agentxcpp
{

/**
 * \brief Manage an SNMP table.
 *
 * This class manages a sequence of TableEntry objects. Unlike normal 
 * variables (which inherit from AbstractVariable), the table cannot be 
 * registered with a MasterProxy object. Instead, the table itself holds a 
 * pointer to a MasterProxy object and adds and removes its variables 
 * dynamically.
 *
 * To implement an SNMP table, a Table object is created and provided with
 * - its own OID (which is the "root node" of all contained variables) and
 * - a MasterProxy object.
 * As long as the MasterProxy object is not provided, no entries can be added 
 * or removed from the table.
 * 
 * Each TableEntry object has an index, which must be unique within a table.  
 * The Table class ensures this when adding a TableEntry.
 *
 * The OID's of the variables within a table are derived from the index of 
 * their TableEntry, which in turn is derived from the contents of the index 
 * variables of that very entry. The index variables are normal SNMP variables 
 * which can be changed (either programatically or by an SNMP set operation).  
 * In such a case, the TableEntry's index would change. However, the Table 
 * object is currently not able to detect such a change, i.e.  variables will 
 * keep their OID instead of changing it according to the new index.
 *
 * \todo Table should detect index changes and re-add the variables 
 *       accordingly. Suggestion: AbstractVariable should provide a 
 *       valueChanged() signal, the Table object registers to this signal for 
 *       all variables in an entry. If the signal is fired, the table checks
 *       whether the TableEntry index changed, and if so, re-adds the 
 *       variables. I think QSignalMapper is needed.
 */
class Table
{
    public:

        /**
         * \brief Constructor.
         *
         * Create a Table object, assign it an OID and associate it with a 
         * MasterProxy object. If master is 0, no TableEntry's may be added.
         *
         * \param oid The tables OID.
         *
         * \param master The MasterProxy which is used by the table to add 
         *               variables.
         *
         * \exception None.
         */
        Table(Oid oid = Oid(), MasterProxy* master = 0)
        : myMasterProxy(master),
          myOid(oid)
        {
        }

        /**
         * \brief Set the OID of the table.
         *
         * This OID is used to build up the full OID of the variables 
         * contained in the table.
         *
         * \note The OID should not be changed while TableEntry objects are 
         *       managed by the Table object. If this happens, the MasterProxy 
         *       will provide access to the variables added before the OID 
         *       change infinitely. The only way to remove those variables is 
         *       to do so manually, using e.g. 
         *       MasterProxy::remove_variable().
         *
         * \param oid The new table OID.
         *
         * \exception None.
         *
         * \todo Provide a clear() method to cleanly remove all tableEntry's
         *
         * \todo Re-add managed variables in case of OID change.
         */
        void setOid(Oid oid)
        {
            myOid = oid;
        }

        /**
         * \brief Get the table's OID.
         *
         * \return The OID used for the table.
         *
         * \exception None.
         */
        Oid oid()
        {
            return myOid;
        }

        /**
         * \brief Set the MasterProxy object used by this table.
         *
         * \param master The new MasterProxy object.
         * 
         * \note The MasterProxy should not be changed while TableEntry 
         *       objects are managed by the Table object. If this happens, the 
         *       old MasterProxy will provide access to the variables added 
         *       before infinitely. The only way to remove those variables is 
         *       to do so manually, using e.g.  
         *       MasterProxy::remove_variable().
         *
         * \exception None.
         *
         * \todo: Remove variables from master, re-add them at new master.
         */
        void setMasterProxy(MasterProxy* master)
        {
            myMasterProxy = master;
        }

        /**
         * \brief Get the MasterProxy object associated with this table.
         *
         * \return The MasterProxy used for by table. May be the NULL pointer.
         *
         * \exception None.
         */
        MasterProxy* masterProxy()
        {
            return myMasterProxy;
        }

        /**
         * \brief Add an entry to the table.
         *
         * This method adds all variables of the entry to the MasterProxy.
         *
         * This function fails in the following cases:
         * - no MasterProxy is currently associated with the table
         * - the entry was already added.
         *
         * \param entry The TableEntry object to add.
         *
         * \return true on success, false otherwise.
         *
         * \exception None.
         */
        bool addEntry(QSharedPointer<TableEntry> entry);

        /**
         * \brief Remove an entry from the table.
         *
         * This method removes all variables of the entry from the 
         * MasterProxy.
         *
         * This function fails in the following cases:
         * - no MasterProxy is currently associated with the table
         * - the entry is not known to the table (it was not added, or it was 
         *   already removed).
         *
         * \param entry The TableEntry object to remove.
         *
         * \return true on success, false otherwise.
         *
         * \exception None.
         */
        bool removeEntry(QSharedPointer<TableEntry> entry);

    private:
        /**
         * \brief The managed TableEntry objects.
         *
         * For each entry the following data is stored:
         * - a pointer to the TableEntry object and
         * - the TableEntry's index at the time it was added to the table.
         */
        QMap< QSharedPointer<TableEntry>, Oid > entries;

        /**
         * \brief The used MasterProxy object.
         *
         * May be NULL.
         */
        MasterProxy* myMasterProxy;

        /**
         * \brief The tables OID.
         */
        Oid myOid;
};

} /* namespace agentxcpp */
#endif /* _TABLE_HPP_ */
