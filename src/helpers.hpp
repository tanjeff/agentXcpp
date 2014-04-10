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
#ifndef _UPTIME_HPP_
#define _UPTIME_HPP_

#include "TimeTicksVariable.hpp"
#include "OidVariable.hpp"

namespace agentxcpp
{
    /**
     * \brief Calculate the uptime of the current process.
     *
     * This function calculates the uptime of the current process, in
     * hundreths of a second. The result may be given as sysUpTime.0.
     * parameter to \ref agentxcpp::MasterProxy::send_notification(
     * const OidVariable&, TimeTicksVariable*,
     * const vector<varbind>&).
     *
     * \internal
     * The time is measured using a global variable which is
     * initialized to the current time just be before main() starts.
     * \endinternal
     *
     * \return The current uptime, in hundreths of a second.
     */
    TimeTicksVariable processUpTime();

    /**
     * \brief The allowed values for specific-trap (SNMPv1 trap).
     *
     * According to RFC 1157, these values can be used for the
     * generic-trap field in SNMPv1 Traps.
     */
    enum generic_trap_t
    {
        coldStart=0,
        warmStart=1,
        linkDown=2,
        linkUp=3,
        authenticationFailure=4,
        egpNeighborLoss=5,
        enterpriseSpecific=6
    };

    /**
     * \brief Create snmpTrapOID.0 value from SNMPv1 trap data.
     *
     * Each notification must have an snmpTrapOID.0 object. In SNMPv1
     * no notifications were available; traps were used instead.  This
     * function takes parameters which would be present in an SNMPv1
     * trap and converts them to the corresponding an snmpTrapOID.0
     * value, which can be included in a notification. The master agent
     * extracts the SNMPv1 trap information while generating a trap (only
     * if sending an SNMPv1 trap, of course).
     *
     * The conversion is done according to RFC 1908,
     * 3.1.2 "SNMPv1 -> SNMPv2".
     *
     * \param generic_trap The type of trap. If it is
     *                     <tt>enterpriseSpecific</tt>, then the
     *                     parameter <tt>specific_trap</tt> must also be given.
     *
     * \param specific_trap Only used if generic_trap is
     *                      <tt>enterpriseSpecific</tt>. It can be any
     *                      value specific to the subagent.
     *
     * \return The value of the snmpTrapOID.0 object.
     *
     * \exception inval_param If the generic_trap parameter has an
     *                        invalid value.
     */
    Oid generate_v1_snmpTrapOID(generic_trap_t generic_trap,
                                     quint32 specific_trap=0);


} // namespace agentxcpp

#endif // _UPTIME_HPP_
