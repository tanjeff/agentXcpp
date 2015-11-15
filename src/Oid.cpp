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

#include <sstream>
#include "Oid.hpp"
#include "exceptions.hpp"


using namespace agentxcpp;
using namespace std;


void Oid::parseString(std::string s)
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



Oid::Oid(std::string s)
{
    // parse the string. Forward all exceptions.
    parseString(s);
}


Oid::Oid(const Oid& o, std::string id)
{
    // start with o
    *this = o;

    // add OID from string. Forward all exceptions.
    parseString(id);
}


Oid::Oid(const Oid& o, quint32 id)
{
    // start with o
    *this = o;

    // add suboid
    append(id);
}


std::ostream& agentxcpp::operator<<(std::ostream& out, const Oid& o)
{
    // Leading dot
    out << ".";

    // If no subidentifiers are present, we are done
    if(o.size() == 0)
    {
	return out;
    }

    // Get iterator to first subidentifier
    Oid::const_iterator it = o.begin();

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


bool Oid::operator<(const Oid& o) const
{
    Oid::const_iterator mine, yours;
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



bool Oid::operator==(const Oid& o) const
{
    // Quick test: if the OidValues have different number of parts, they are not 
    // equal:
    if( this->size() != o.size() )
    {
	return false;
    }
    
    // Test all parts:
    Oid::const_iterator mine, yours;
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


Oid& Oid::operator=(const Oid& other)
{
    // copy inherited stuff
    QVector<quint32>::operator=(other);
    
    mInclude = other.mInclude;

    // Return reference to us
    return *this;
}


bool Oid::contains(const Oid& id) const
{
    // If id has fewer subids than this: not contained
    if(this->size() > id.size())
    {
	// Is not contained
	return false;
    }

    // id has at least as many subids than this -> iteration is safe
    for(int i = 0; i < this->size(); i++)
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


bool Oid::is_null() const
{
    if( this->size() == 0 &&
	! this->mInclude)
    {
	// Is the null OID
	return true;
    }
    else
    {
	return false;
    }
}
