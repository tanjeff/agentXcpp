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

#include <iostream>
#include "oid.hpp"
using namespace agentxcpp;


oid::oid(int c1, int c2, int c3, int c4, int c5, int c6,
	 int c7, int c8, int c9, int c10, int c11, int c12,
	 int c13, int c14, int c15, int c16, int c17, int c18,
	 int c19, int c20)
{
    include = false;

    if(c1) identifier.push_back(c1); else return;
    if(c2) identifier.push_back(c2); else return;
    if(c3) identifier.push_back(c3); else return;
    if(c4) identifier.push_back(c4); else return;
    if(c5) identifier.push_back(c5); else return;
    if(c6) identifier.push_back(c6); else return;
    if(c7) identifier.push_back(c7); else return;
    if(c8) identifier.push_back(c8); else return;
    if(c9) identifier.push_back(c9); else return;
    
    if(c10) identifier.push_back(c10); else return;
    if(c11) identifier.push_back(c11); else return;
    if(c12) identifier.push_back(c12); else return;
    if(c13) identifier.push_back(c13); else return;
    if(c14) identifier.push_back(c14); else return;
    if(c15) identifier.push_back(c15); else return;
    if(c16) identifier.push_back(c16); else return;
    if(c17) identifier.push_back(c17); else return;
    if(c18) identifier.push_back(c18); else return;
    if(c19) identifier.push_back(c19); else return;
    
    if(c20) identifier.push_back(c20); else return;
}

oid oid::operator+(int component) const
{
    oid new_oid = *this;
    new_oid.identifier.push_back(component);
    return new_oid;
}



std::ostream& agentxcpp::operator<<(std::ostream& out, const oid& o)
{
    if(o.identifier.size() == 0)
    {
	return out;
    }
    std::vector<uint32_t>::const_iterator it;
    it = o.identifier.begin();

    out << *it;
    it++;

    while(it != o.identifier.end())
    {
	out << "." << *it;
	it++;
    }
    return out;
}



data_t oid::serialize() const
{
    // The serial representation of an OID is as follows (RFC 2741, section 
    // 5.1):
    //
    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    // |  n_subid      |  prefix       |  include      |  <reserved>   |
    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    // |                       sub-identifier #1                       |
    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    // |                       sub-identifier #n_subid                 |
    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    //
    // We use a string here to represent the serial stream, an we use some 
    // constants as indexes:
    const int n_subid_idx = 0;
    const int prefix_idx = 1;
    const int include_idx = 2;
    const int reserved_idx = 2;

    // This is our binary data:
    data_t serialized_oid;
    serialized_oid.resize(4);	// we will need at least the header

    // Set reserved field to 0
    serialized_oid[reserved_idx] = 0;

    // Set include field
    serialized_oid[include_idx] = include ? 1 : 0;

    // Iterator for the subid's
    std::vector<uint32_t>::const_iterator subid = identifier.begin();

    // Check whether we can use the prefix (RFC 2741, section 5.1)
    if( identifier.size() >= 5 &&
	identifier[0] == 1 &&
	identifier[1] == 3 &&
	identifier[2] == 6 &&
	identifier[3] == 1 &&
	identifier[4] <= 0xff)	// we have only one byte for the prefix!
    {
	// store the first integer after 1.3.6.1 to prefix field
	serialized_oid[prefix_idx] = identifier[4];
	subid += 5; // point to the subid behind prefix

	// 5 elements are represented by prefix
	serialized_oid[n_subid_idx] = identifier.size() - 5;

    }
    else
    {
	// don't use prefix field
	serialized_oid[prefix_idx] = 0;

	// All subid's are stored in the stream explicitly
	serialized_oid[n_subid_idx] = identifier.size();
    }

    // copy subids to serialized_oid
    while( subid != identifier.end() )
    {
	serialized_oid.push_back( (*subid) << 24 & 0xff );
	serialized_oid.push_back( (*subid) << 16 & 0xff );
	serialized_oid.push_back( (*subid) << 8 & 0xff );
	serialized_oid.push_back( (*subid) << 0 & 0xff );
	subid++;
    }

    return serialized_oid;
}

oid::oid(data_t::const_iterator& pos,
	 const data_t::const_iterator& end,
	 bool big_endian)
{
    if(end - pos < 4)
    {
	throw(parse_error());
    }
    
    // get number of subid's
    int n_subid = *pos++;

    // parse prefix
    int prefix = *pos++;
    if( prefix != 0 )
    {
	identifier.push_back(1);
	identifier.push_back(3);
	identifier.push_back(6);
	identifier.push_back(1);
	identifier.push_back(prefix);
    }

    // parse include field
    switch( *pos++ )
    {
	case 0:
	    include = false;
	    break;
	case 1:
	    include = true;
	    break;
	default:
	    // Invalid value; we are picky and indicate an error:
	    throw parse_error();
    }

    // skip reserved field
    *pos++;

    // parse rest of data, subid by subid
    if(end - pos < n_subid * 4)
    {
	throw(parse_error());
    }
    uint32_t subid;
    for( int i = 0; i < n_subid; i++)
    {
	if(big_endian)
	{
	    // big endian
	    subid =  *pos++ << 24;
	    subid |= *pos++ << 16;
	    subid |= *pos++ << 8;
	    subid |= *pos++ << 0;
	}
	else
	{
	    // little posdian
	    subid =  *pos++ << 0;
	    subid |= *pos++ << 8;
	    subid |= *pos++ << 16;
	    subid |= *pos++ << 24;
	}
	identifier.push_back(subid);
    }
}


bool oid::operator<(const oid& o) const
{
    std::vector<uint32_t>::const_iterator mine, yours;
    mine = this->identifier.begin();
    yours = o.identifier.begin();

    // Test as many parts as the shorter OID has:
    while( mine != this->identifier.end()
	    && yours != o.identifier.end() )
    {
	if( *mine < *yours )
	{
	    // my oid part is less than yours
	    return true;
	}
	if( *mine > *yours )
	{
	    // my oid part is greater than yours
	    return false;
	}

	// our parts are identical; test next art.:
	mine++;
	yours++;
    }

    // Ok, either you and I have different length (where the one with fewer 
    // parts is less than the other) or we have the same number of parts (in 
    // which case we are identical).
    if( this->identifier.size() < o.identifier.size() )
    {
	// I have less parts than you, so I am less than you:
	return true;
    }
    else
    {
	// I have not less parts than you:
	return false;
    }
}

bool oid::operator==(const oid& o) const
{
    // Quick test: if the oids have different number of parts, they are not 
    // equal:
    if( this->identifier.size() != o.identifier.size() )
    {
	return false;
    }
    
    // Test all parts:
    std::vector<uint32_t>::const_iterator mine, yours;
    mine = this->identifier.begin();
    yours = o.identifier.begin();

    while( mine != this->identifier.end()
	    && yours != o.identifier.end() )
    {
	if( *mine != *yours )
	{
	    // The parts differ: OIDs not equal
	    return false;
	}

	// Parts are equal, test next parts
	mine++;
	yours++;
    }

    // All parts tested and all parts were equal. Further both OIDs have the 
    // same number of parts, thus they are equal.
    return true;
}
