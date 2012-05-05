#include "ResponsePDU.hpp"
#include "helper.hpp"

using namespace agentxcpp;


void ResponsePDU::set_error(error_t e)
{
    /* We accept only allowed values: */
    switch(e)
    {
	// fall-through technology :-)
	case noAgentXError:
	case openFailed:
	case notOpen:
	case indexWrongType:
	case indexAlreadyAllocated:
	case indexNoneAvailable:
	case indexNotAllocated:
	case unsupportedContext:
	case duplicateRegistration:
	case unknownRegistration:
	case unknownAgentCaps:
	case parseError:
	case requestDenied:
	case processingError:
	    error = e;
	    break;
	default:
	    throw(inval_param());
    }
}


ResponsePDU::ResponsePDU()
{
    this->error = noAgentXError;
    this->index = 0;
    this->sysUpTime = 0;
    // varbindlist is automatically initialized to the empty list
}



ResponsePDU::ResponsePDU(binary::const_iterator& pos,
			 const binary::const_iterator& end,
			 bool big_endian)
    : PDU(pos, end, big_endian)
{
    // header is parsed by base class constructor

    uint16_t err;

    // read simple fields
    this->sysUpTime = read32(pos, big_endian);
    err = read16(pos, big_endian);
    this->index = read16(pos, big_endian);

    // read varbindlist if present
    while(pos != end)
    {
	varbindlist.push_back(varbind(pos, end, big_endian));
    }
    
    switch(err)
    {
	// fall-through technology :-)
	case noAgentXError:
	case openFailed:
	case notOpen:
	case indexWrongType:
	case indexAlreadyAllocated:
	case indexNoneAvailable:
	case indexNotAllocated:
	case unsupportedContext:
	case duplicateRegistration:
	case unknownRegistration:
	case unknownAgentCaps:
	case parseError:
	case requestDenied:
	case processingError:
	    this->error = static_cast<error_t>(err);
	    break;
	default:
	    throw(inval_param());
    }

}



binary ResponsePDU::serialize() const
{
    binary serialized;

    // Encode simple fields
    write32(serialized, this->sysUpTime);
    write16(serialized, this->error); 
    write16(serialized, this->index);

    // Encode VarBindList
    vector<varbind>::const_iterator i;
    for(i = this->varbindlist.begin(); i != this->varbindlist.end(); i++)
    {
	serialized += i->serialize();
    }

    // Add Header
    add_header(PDU::agentxResponsePDU, serialized);

    return serialized;
}
