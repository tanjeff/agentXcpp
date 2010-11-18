#ifndef _VARBIND_H_
#define _VARBIND_H_

#include "types.h"
#include "oid.h"
#include "variable.h"

namespace agentx
{
    class varbind
    {
	private:
	    oid* name;
	    variable* var;
	public:

	    varbind(oid*, variable* v);
	    data_t serialize();
    };

}
#endif
