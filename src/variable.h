#ifndef _VARIABLE_H_
#define _VARIABLE_H_

#include "types.h"

/**
 * \internal
 *
 * \brief This class represents a SNMP variable.
 *
 * It is inherited by more concrete classes.
 */
class variable
{
    public:
	//virtual data_t serialize();
	//virtual void deserialize(data_t);
	
	// Needed for encoding varbinds
	virtual data_t serialize() = 0;
};


#endif
