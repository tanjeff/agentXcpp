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
    std::vector<int>::const_iterator it;
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
