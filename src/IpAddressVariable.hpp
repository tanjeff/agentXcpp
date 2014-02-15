#ifndef _IPADDRESSVARIABLE_H_
#define _IPADDRESSVARIABLE_H_

#include "IpAddressValue.hpp"
#include "Variable.hpp"

namespace agentxcpp
{

    class IpAddressVariable : public Variable<IpAddressValue>
    {
        private:

            IpAddressValue v;

        public:

            IpAddressVariable(IpAddressValue _value)
        : v(_value)
        {
        }

            void setValue(IpAddressValue _value)
            {
                v = _value;
            }

            virtual IpAddressValue get()
            {
                return v;
            }

            virtual IpAddressValue value()
            {
                return v;
            }

    };
}
#endif // _IPADDRESSVARIABLE_H_
