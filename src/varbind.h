#ifndef _VARBIND_H_
#define _VARBIND_H_

#include "types.h"
#include "oid.h"
#include "variable.h"

namespace agentx
{
    /**
     * \brief Represents a VarBind according to RFC 2741, section 5.4.
     */
    class varbind
    {
	private:
	    /**
	     * \brief The name (OID) of the VarBind.
	     */
	    oid* name;

	    /**
	     * \brief The variable inside the varbind.
	     *
	     * This pointer may be 0 if the varbind has a type without a 
	     * variable (e.g. "NoSuchObject").
	     */
	    variable* var;

	    /**
	     * \brief The type of the varbind.
	     *
	     * This field represents the type as described in RFC 2741, section 
	     * 5.4. The serialize() function will copy it directly into the 
	     * varbind.
	     */
	    uint16_t type;
	public:

	    /**
	     * \brief Create a VarBind with an oid and a var.
	     *
	     * The variable must be one of the following types:
	     * - Integer
	     * - octet_string
	     * - oid
	     * - IpAddress
	     * - Counter32
	     * - Gauge32
	     * - TimeTicks
	     * - Opaque
	     * - Counter64
	     * If the type of the variable cannot be determined, inval_param is 
	     * thrown.
	     */
	    varbind(oid*, variable* v) throw(inval_param);
	    
	    /**
	     * \brief These values can be used to create a VarBind.
	     */
	    enum type_t {
		Null = 5,
		noSuchObject = 128,
		noSuchInstance = 129,
		endOfMibView = 130
	    };


	    /**
	     * \brief Create VarBind without var, but with a type.
	     *
	     * Only the constants defined by varbind::type_t are allowed.  A 
	     * wrong type will cause an inval_param exception.
	     */
	    varbind(oid*, type_t) throw(inval_param);

	    data_t serialize();
    };

}
#endif
