#ifndef _OIDVARIABLE_H_
#define _OIDVARIABLE_H_

#include "OidValue.hpp"
#include "Variable.hpp"

namespace agentxcpp
{

    class OidVariable : public Variable<OidValue>
    {
        private:

            OidValue v;

        public:

            OidVariable(OidValue _value = OidValue())
        : v(_value)
        {
        }

            void setValue(OidValue _value)
            {
                v = _value;
            }

            virtual OidValue get()
            {
                return v;
            }

            virtual OidValue value()
            {
                return v;
            }

    };
}
#endif // _OIDVARIABLE_H_
