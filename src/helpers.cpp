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
#include <QDateTime>

#include "helpers.hpp"

using namespace agentxcpp;

namespace agentxcpp
{
    /**
     * \internal
     *
     * \brief Variable to measure the uptime of the current process.
     *
     * This variable is initialized when the executable starts up
     * and holds the time at which this happened. Afterwards,
     * the uptime of the process can be calculated.
     */
    static QDateTime process_start_time(QDateTime::currentDateTime());
}

    TimeTicksVariable agentxcpp::processUpTime()
    {
        // Calculate uptime
        qint64 uptime = process_start_time.msecsTo(QDateTime::currentDateTime());

        // Convert uptime to hundreths of seconds
        TimeTicksVariable sysuptime( uptime/10 );

        // Return result
        return sysuptime;
    }

    Oid agentxcpp::generate_v1_snmpTrapOID(generic_trap_t generic_trap,
                                                quint32 specific_trap)
    {
        // We need the OID of the SNMPv1 traps. These are defined here.
        //
        // First we define a "helper" OID:
        static const Oid snmpTraps_oid(snmpMIBObjects_oid, "5");
        //
        // Some traps according to RFC 1907:
        static const Oid snmpTraps_coldStart_oid(snmpTraps_oid, "1");
        static const Oid snmpTraps_warmStart_oid(snmpTraps_oid, "2");
        static const Oid snmpTraps_authenticationFailure_oid(snmpTraps_oid, "5");
        //
        // Some traps according to RFC 1573:
        static const Oid snmpTraps_linkDown_oid(snmpTraps_oid, "3");
        static const Oid snmpTraps_linkUp_oid(snmpTraps_oid, "4");

        // Finally, egpNeighborLoss. According to RC 1907 it is defined in RFC
        // 1213, however, the latter doesn't define it. On the other hand,
        // RFC 2089 defines egpNeighborLoss as 1.3.6.1.6.3.1.1.5.6, which is
        // snmpTraps.6 and corresponds to the comment in RFC 1907, so we use this
        // one:
        static const Oid snmpTraps_egpNeighborLoss_oid(snmpTraps_oid, "6");

        // calculate the value of snmpTrapOID.0 according to RFC 1908:
        Oid value;

        switch(generic_trap)
        {
            case coldStart:
                value = snmpTraps_coldStart_oid;
                break;
            case warmStart:
                value = snmpTraps_warmStart_oid;
                break;
            case linkDown:
                value = snmpTraps_linkDown_oid;
                break;
            case linkUp:
                value = snmpTraps_linkUp_oid;
                break;
            case authenticationFailure:
                value = snmpTraps_authenticationFailure_oid;
                break;
            case egpNeighborLoss:
                value = snmpTraps_egpNeighborLoss_oid;
                break;
            case enterpriseSpecific:
                value = enterprises_oid;
                value.push_back(0);
                value.push_back(specific_trap);
                break;
            default:
                // invalid generic_trap value!
                throw(inval_param());
        }

        // Create and return varbind
        return value;
    }
