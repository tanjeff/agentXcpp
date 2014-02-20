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

//#include <iostream>
#include <sstream>
#include "OidVariable.hpp"
#include "exceptions.hpp"


using namespace agentxcpp;
using namespace std;


void OidVariable::parseString(std::string s)
{
    // Do not parse empty string
    if(s.empty()) return;

    // Parse the string
    std::istringstream ss(s);
    quint32 subid;
    char ch;
    while(ss)
    {
	// Read a subid
	ss >> subid;
	if(!ss)
	{
	    // cannot get number: parse error
	    // This happens also if the number is too large
	    throw( inval_param() );
	}
	push_back(subid);

	// Read a period
	ss >> ch;
	if(!ss)
	{
	    // end of string: end of parsing
	    break;
	}
	if(ch != '.')
	{
	    // Wrong char: parse error
	    throw( inval_param() );
	}
    }
}



OidVariable::OidVariable(std::string s)
{
    include = false;

    // parse the string. Forward all exceptions.
    parseString(s);
}


OidVariable::OidVariable(const OidVariable& o, std::string id)
{
    // start with o
    *this = o;

    // add OID from string. Forward all exceptions.
    parseString(id);
}


std::ostream& agentxcpp::operator<<(std::ostream& out, const OidVariable& o)
{
    // Leading dot
    out << ".";

    // If no subidentifiers are present, we are done
    if(o.size() == 0)
    {
	return out;
    }

    // Get iterator to first subidentifier
    OidVariable::const_iterator it = o.begin();

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



binary OidVariable::serialize() const
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
    // We use a string here to represent the serial stream, and we use some 
    // constants as indexes:
    const int n_subid_idx = 0;
    const int prefix_idx = 1;
    const int include_idx = 2;
    const int reserved_idx = 3;

    // This is our binary data:
    binary serialized;
    serialized.resize(4);	// we will need at least the header

    // Set reserved field to 0
    serialized[reserved_idx] = 0;

    // Set include field
    serialized[include_idx] = include ? 1 : 0;

    // Iterator for the subid's
    OidVariable::const_iterator subid = this->begin();

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

OidVariable::OidVariable(binary::const_iterator& pos,
	 const binary::const_iterator& end,
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
    quint32 subid;
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


bool OidVariable::operator<(const OidVariable& o) const
{
    OidVariable::const_iterator mine, yours;
    mine = this->begin();
    yours = o.begin();

    // Test as many parts as the shorter OID has:
    while( mine != this->end()
	    && yours != o.end() )
    {
	if( *mine < *yours )
	{
	    // my OidValue part is less than yours
	    return true;
	}
	if( *mine > *yours )
	{
	    // my OidValue part is greater than yours
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



bool OidVariable::operator==(const OidVariable& o) const
{
    // Quick test: if the OidValues have different number of parts, they are not 
    // equal:
    if( this->size() != o.size() )
    {
	return false;
    }
    
    // Test all parts:
    OidVariable::const_iterator mine, yours;
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


OidVariable& OidVariable::operator=(const OidVariable& other)
{
    // copy our own members
    this->include = other.include;

    // copy inherited stuff
    QVector<quint32>::operator=(other);
    AbstractVariable::operator=(other);
    
    // Return reference to us
    return *this;
}


bool OidVariable::contains(const OidVariable& id)
{
    // If id has fewer subids than this: not contained
    if(this->size() > id.size())
    {
	// Is not contained
	return false;
    }

    // id has at least as many subids than this -> iteration is safe
    for(size_type i = 0; i < this->size(); i++)
    {
	if( (*this)[i] != id[i] )
	{
	    // We differ in a subid!
	    return false;
	}
    }

    // If we get here, the id starts the same subids as this (it has possibly 
    // more subids). This means that it is contained in the subtree spanned by 
    // this.
    return true;
}


bool OidVariable::is_null() const
{
    if( this->size() == 0 &&
	! this->include)
    {
	// Is the null OID
	return true;
    }
    else
    {
	return false;
    }
}
