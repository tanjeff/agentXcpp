#ifndef _OPAQUEVARIABLE_H_
#define _OPAQUEVARIABLE_H_

#include "OpaqueValue.hpp"
#include "Variable.hpp"

namespace agentxcpp
{

    class OpaqueVariable : public Variable<OpaqueValue>
    {
        private:

            OpaqueValue v;

        public:

            OpaqueVariable(OpaqueValue _value = OpaqueValue())
        {
                v = _value;
        }

            void setValue(OpaqueValue _value)
            {
                v = _value;
            }

            virtual OpaqueValue get()
            {
                return v;
            }

            virtual OpaqueValue value()
            {
                return v;
            }

    };
}
#endif // _OPAQUEVARIABLE_H_
