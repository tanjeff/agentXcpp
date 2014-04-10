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
#include "OidVariable.hpp"
#include "exceptions.hpp"


using namespace agentxcpp;
using namespace std;


OidVariable::OidVariable(const Oid& o)
{
    mValue = o;
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
    serialized[include_idx] = mValue.include() ? 1 : 0;

    // Iterator for the subid's
    Oid::const_iterator subid = mValue.begin();

    // Check whether we can use the prefix (RFC 2741, section 5.1)
    if( mValue.size() >= 5 &&
	mValue[0] == 1 &&
	mValue[1] == 3 &&
	mValue[2] == 6 &&
	mValue[3] == 1 &&
	mValue[4] <= 0xff)	// we have only one byte for the prefix!
    {
	// store the first integer after 1.3.6.1 to prefix field
	serialized[prefix_idx] = mValue[4];
	subid += 5; // point to the subid behind prefix

	// 5 elements are represented by prefix
	serialized[n_subid_idx] = mValue.size() - 5;
    }
    else
    {
	// don't use prefix field
	serialized[prefix_idx] = 0;

	// All subid's are stored in the stream explicitly
	serialized[n_subid_idx] = mValue.size();
    }

    // copy subids to serialized
    while( subid != mValue.end() )
    {
	serialized.push_back( (*subid) >> 24 & 0xff );
	serialized.push_back( (*subid) >> 16 & 0xff );
	serialized.push_back( (*subid) >> 8 & 0xff );
	serialized.push_back( (*subid) >> 0 & 0xff );
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
	mValue.push_back(1);
	mValue.push_back(3);
	mValue.push_back(6);
	mValue.push_back(1);
	mValue.push_back(prefix);
    }

    // parse include field
    switch( *pos++ )
    {
	case 0:
	    mValue.setInclude(false);
	    break;
	case 1:
	    mValue.setInclude(true);
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
	mValue.push_back(subid);
    }
}


