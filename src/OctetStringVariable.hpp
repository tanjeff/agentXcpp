#ifndef _OCTETSTRINGVARIABLE_H_
#define _OCTETSTRINGVARIABLE_H_

#include "OctetStringValue.hpp"
#include "Variable.hpp"

namespace agentxcpp
{

    class OctetStringVariable : public Variable<OctetStringValue>
    {
        private:

            OctetStringValue v;

        public:

            OctetStringVariable(binary _value = binary())
        : v(_value)
        {
        }

            void setValue(binary _value)
            {
                v.set_value(_value);
            }

            virtual OctetStringValue get()
            {
                return v;
            }

            virtual binary value()
            {
                return v.get_value();
            }

    };
}
#endif // _OCTETSTRINGVARIABLE_H_
