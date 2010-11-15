#ifndef _OCTET_STRING_H_
#define _OCTET_STRING_H_

#include "types.h"
#include "variable.h"

class octet_string : public variable
{
    private:
	data_t value;
    public:
	data_t serialize();
	
	virtual uint16_t syntax()
	{
	    return 4;	// RFC 2741, section 5.4
	}
	
	virtual data_t get() = 0;
};

#endif
