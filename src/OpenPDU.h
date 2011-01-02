#ifndef __OPENPDU_H__
#define __OPENPDU_H__

//#include "types.h"
//#include "exceptions.h"
//#include "Octet_String.h"
#include "PDU.h"

namespace agentx
{
    /**
     * \brief Represents an Open-PDU
     *
     */
    class OpenPDU : public PDU
    {
	public:
	    OpenPDU(data_t::const_iterator& pos, bool big_endian);
    };
}


#endif
