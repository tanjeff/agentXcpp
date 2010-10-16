#include <iostream>
#include "oid.h"
using namespace agentx;


oid::oid(int c1, int c2, int c3, int c4, int c5, int c6,
	 int c7, int c8, int c9, int c10, int c11, int c12,
	 int c13, int c14, int c15, int c16, int c17, int c18,
	 int c19, int c20)
{
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

oid& oid::operator,(int component)
{
    identifier.push_back(component);
}



std::ostream& agentx::operator<<(std::ostream& out, const oid& o)
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



std::basic_string<uint8_t> oid::serialize(uint8_t _include)
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
    const int n_subid = 0;
    const int prefix = 1;
    const int include = 2;
    const int reserved = 2;

    // This is our binary data:
    std::basic_string<uint8_t> serialized_oid;
    serialized_oid.resize(4);	// we will need at least the header

    // Set reserved field to 0
    serialized_oid[reserved] = 0;

    // Set include field
    serialized_oid[include] = _include;

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
	serialized_oid[prefix] = identifier[4];
	subid += 5; // point to the subid behind prefix

	// 5 elements are represented by prefix
	serialized_oid[n_subid] = identifier.size() - 5;

    }
    else
    {
	// don't use prefix field
	serialized_oid[prefix] = 0;

	// All subid's are stored in the stream explicitly
	serialized_oid[n_subid] = identifier.size();
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


void oid::deserialize(std::basic_string<uint8_t> data) throw(parse_error)
{
    if( data.size() < 4)
    {
	// Header missing!
	throw parse_error();
    }

    // get number of subid's
    int n_subid = data[0];

    if( data.size() != 4 + n_subid * 4 )
    {
	// too much/less data!
	throw parse_error();
    }
    
    // parse prefix
    int prefix = data[1];
    if( prefix != 0 )
    {
	identifier.push_back(1);
	identifier.push_back(3);
	identifier.push_back(6);
	identifier.push_back(1);
	identifier.push_back(prefix);
    }

    // parse rest of data, subid by subid
    uint32_t subid;
    for( int i = 0; i < n_subid; i++)
    {
	subid = data[4+i*4 + 0] << 24;
	subid |= data[4+i*4 + 1] << 16;
	subid |= data[4+i*4 + 2] << 8;
	subid |= data[4+i*4 + 3] << 0;
	identifier.push_back(subid);
    }
}
