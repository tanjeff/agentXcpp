#ifndef _VARIABLE_H_
#define _VARIABLE_H_

#include "types.h"

// This class represents a SNMP variable. It is inherited by more concrete 
// classes.
class variable
{
    public:
	//virtual data_t serialize();
	//virtual void deserialize(data_t);

	virtual data_t get() = 0;
};


#endif
