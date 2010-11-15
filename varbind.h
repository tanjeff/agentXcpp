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
	    variable* data;
	public:
	    varbind(oid&);
	    varbind(oid&, data_t&);
	    data_t serialize();
    };

}
#endif
