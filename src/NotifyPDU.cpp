/*
 * Copyright 2011 Tanjeff-Nicolai Moos <tanjeff@cccmz.de>
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

#include "NotifyPDU.hpp"


using namespace agentxcpp;

NotifyPDU::NotifyPDU(data_t::const_iterator& pos,
		     const data_t::const_iterator& end,
		     bool big_endian)
    : PDUwithContext(pos, end, big_endian)
{
    // Get VarBind's until the PDU is completely parsed
    while( pos < end )
    {
	vb.push_back(varbind(pos, end, big_endian));
    }
}
	    



data_t NotifyPDU::serialize() const
{
    data_t serialized;

    // Add VarBind's
    vector<varbind>::const_iterator i;
    for(i = vb.begin(); i < vb.end(); i++)
    {
	serialized += i->serialize();
    }

    // Add header
    add_header(PDU::agentxNotifyPDU, serialized);

    // return serialized form of PDU
    return serialized;
}


varbind NotifyPDU::trapToNotification(oid enterprise,
				      generic_trap_t generic_trap,
				      uint32_t specific_trap)
{
    // The snmpTrapOID type
    //
    // According to RFC 1907:
    // snmpTrapOID ::= snmpTrap.1
    // snmpTrap ::= snmpMIBObjects.4
    // snmpMIBObjects ::= snmpMIB.1
    // snmpMIB ::= snmpModules.1
    //
    // According to RFC 2578 (SNMPv2-SMI):
    // snmpModules ::= snmpV2.3
    // snmpV2 ::= internet.6
    // 
    // conclusion:
    static const oid snmpMIBObjects(internet,6,3,1,1);
    static const oid snmpTrapOID(snmpMIBObjects,4,1);


    // We need the OID of the SNMPv1 traps. These are defined here.
    //
    // First we define a "helper" OID:
    static const oid snmpTraps(snmpMIBObjects,5);
    //
    // Some traps according to RFC 1907:
    static const oid snmpTraps_coldStart(snmpTraps,1);
    static const oid snmpTraps_warmStart(snmpTraps,2);
    static const oid snmpTraps_authenticationFailure(snmpTraps,5);
    //
    // Some traps according to RFC 1573:
    static const oid snmpTraps_linkDown(snmpTraps,3);
    static const oid snmpTraps_linkUp(snmpTraps,4);
    //
    // Finally, egpNeighborLoss. According to RC 1907 it is defined in RFC 
    // 1213, however, the latter doesn't in fact define it. On the other hand, 
    // RFC 2089 defines egpNeighborLoss as 1.3.6.1.6.3.1.1.5.6, which is 
    // snmpTraps.6 and corresponds to the comment in RFC 1907, so we use this 
    // one:
    static const oid snmpTraps_egpNeighborLoss(snmpTraps,6);


    // calculate the value of snmpTrapOID.0 according to RFC 1908
    oid* value;

    switch(generic_trap)
    {
	case coldStart:
	    value = new oid(coldStart);
	    break;
	case warmStart:
	    value = new oid(warmStart);
	    break;
	case linkDown:
	    value = new oid(linkDown);
	    break;
	case linkUp:
	    value = new oid(linkUp);
	    break;
	case authenticationFailure:
	    value = new oid(authenticationFailure);
	    break;
	case egpNeighborLoss:
	    value = new oid(egpNeighborLoss);
	    break;
	case enterpriseSpecific:
	    value = new oid(enterprise, 0, specific_trap);
	    break;
	default:
	    // invalid generic_trap value!
	    throw(inval_param());
    }

    // Create and return varbind
    return varbind(oid(snmpTrapOID,0), value);
}
