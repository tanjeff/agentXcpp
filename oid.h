#ifndef _OID_H_
#define _OID_H_

#include <vector>
#include <ostream>

namespace agentx
{

    class oid
    {
	std::vector<int> identifier;

	public:
	typedef int component_type;

	// TODO: How wide are the integers for OIDs?
	oid(int c1=0, int c2=0, int c3=0, int c4=0, int c5=0, int c6=0,
	    int c7=0, int c8=0, int c9=0, int c10=0, int c11=0, int c12=0,
	    int c13=0, int c14=0, int c15=0, int c16=0, int c17=0, int c18=0,
	    int c19=0, int c20=0 );
	oid& operator,(int);
    
	friend std::ostream& operator<<(std::ostream&, const agentx::oid&);

    };
    
    std::ostream& operator<<(std::ostream&, const agentx::oid&);

}


#endif
