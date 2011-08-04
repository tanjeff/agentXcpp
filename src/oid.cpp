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

//#include <iostream>
#include "oid.hpp"
#include "exceptions.hpp"


using namespace agentxcpp;


oid::oid(uint32_t  c1, uint32_t  c2, uint32_t  c3,
	 uint32_t  c4, uint32_t  c5, uint32_t  c6,
	 uint32_t  c7, uint32_t  c8, uint32_t  c9,
	 uint32_t c10, uint32_t c11, uint32_t c12,
	 uint32_t c13, uint32_t c14, uint32_t c15,
	 uint32_t c16, uint32_t c17, uint32_t c18,
	 uint32_t c19, uint32_t c20)
{
    include = false;

    if(c1) push_back(c1); else return;
    if(c2) push_back(c2); else return;
    if(c3) push_back(c3); else return;
    if(c4) push_back(c4); else return;
    if(c5) push_back(c5); else return;
    if(c6) push_back(c6); else return;
    if(c7) push_back(c7); else return;
    if(c8) push_back(c8); else return;
    if(c9) push_back(c9); else return;
    
    if(c10) push_back(c10); else return;
    if(c11) push_back(c11); else return;
    if(c12) push_back(c12); else return;
    if(c13) push_back(c13); else return;
    if(c14) push_back(c14); else return;
    if(c15) push_back(c15); else return;
    if(c16) push_back(c16); else return;
    if(c17) push_back(c17); else return;
    if(c18) push_back(c18); else return;
    if(c19) push_back(c19); else return;
    
    if(c20) push_back(c20); else return;
}


oid::oid(const oid& o,
	 uint32_t c1, uint32_t c2, uint32_t c3, uint32_t c4, uint32_t c5,
	 uint32_t c6, uint32_t c7, uint32_t c8, uint32_t c9, uint32_t c10,
	 uint32_t c11, uint32_t c12, uint32_t c13, uint32_t c14, uint32_t c15,
	 uint32_t c16, uint32_t c17, uint32_t c18, uint32_t c19)
{
    // start with o
    *this = o;

    if(c1) push_back(c1); else return;
    if(c2) push_back(c2); else return;
    if(c3) push_back(c3); else return;
    if(c4) push_back(c4); else return;
    if(c5) push_back(c5); else return;
    if(c6) push_back(c6); else return;
    if(c7) push_back(c7); else return;
    if(c8) push_back(c8); else return;
    if(c9) push_back(c9); else return;
    
    if(c10) push_back(c10); else return;
    if(c11) push_back(c11); else return;
    if(c12) push_back(c12); else return;
    if(c13) push_back(c13); else return;
    if(c14) push_back(c14); else return;
    if(c15) push_back(c15); else return;
    if(c16) push_back(c16); else return;
    if(c17) push_back(c17); else return;
    if(c18) push_back(c18); else return;
    if(c19) push_back(c19); else return;
}


std::ostream& agentxcpp::operator<<(std::ostream& out, const oid& o)
{
    // Leading dot
    out << ".";

    // If no subidentifiers are present, we are done
    if(o.size() == 0)
    {
	return out;
    }

    // Get iterator to first subidentifier
    oid::const_iterator it = o.begin();

    // Print first subidentifier
    out << *it;
    it++;

    // Output remaining subidentifiers, each prepended with a dot
    while(it != o.end())
    {
	out << "." << *it;
	it++;
    }

    // Done, return
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
    data_t serialized;
    serialized.resize(4);	// we will need at least the header

    // Set reserved field to 0
    serialized[reserved_idx] = 0;

    // Set include field
    serialized[include_idx] = include ? 1 : 0;

    // Iterator for the subid's
    oid::const_iterator subid = this->begin();

    // Check whether we can use the prefix (RFC 2741, section 5.1)
    if( this->size() >= 5 &&
	(*this)[0] == 1 &&
	(*this)[1] == 3 &&
	(*this)[2] == 6 &&
	(*this)[3] == 1 &&
	(*this)[4] <= 0xff)	// we have only one byte for the prefix!
    {
	// store the first integer after 1.3.6.1 to prefix field
	serialized[prefix_idx] = (*this)[4];
	subid += 5; // point to the subid behind prefix

	// 5 elements are represented by prefix
	serialized[n_subid_idx] = this->size() - 5;
    }
    else
    {
	// don't use prefix field
	serialized[prefix_idx] = 0;

	// All subid's are stored in the stream explicitly
	serialized[n_subid_idx] = this->size();
    }

    // copy subids to serialized
    while( subid != this->end() )
    {
	serialized.push_back( (*subid) << 24 & 0xff );
	serialized.push_back( (*subid) << 16 & 0xff );
	serialized.push_back( (*subid) << 8 & 0xff );
	serialized.push_back( (*subid) << 0 & 0xff );
	subid++;
    }

    return serialized;
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
	this->push_back(1);
	this->push_back(3);
	this->push_back(6);
	this->push_back(1);
	this->push_back(prefix);
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
	this->push_back(subid);
    }
}


bool oid::operator<(const oid& o) const
{
    oid::const_iterator mine, yours;
    mine = this->begin();
    yours = o.begin();

    // Test as many parts as the shorter OID has:
    while( mine != this->end()
	    && yours != o.end() )
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

	// our parts are identical; test next part:
	mine++;
	yours++;
    }

    // Ok, either you and I have different length (where the one with fewer 
    // parts is less than the other) or we have the same number of parts (in 
    // which case we are identical).
    if( this->size() < o.size() )
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
    if( this->size() != o.size() )
    {
	return false;
    }
    
    // Test all parts:
    oid::const_iterator mine, yours;
    mine = this->begin();
    yours = o.begin();

    while( mine != this->end()
	    && yours != o.end() )
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


oid& oid::operator=(const oid& other)
{
    // copy our own members
    this->include = other.include;

    // copy inherited stuff
    vector<uint32_t>::operator=(other);
    variable::operator=(other);
    
    // Return reference to us
    return *this;
}
