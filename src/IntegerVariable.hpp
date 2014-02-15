#ifndef _INTEGERVARIABLE_H_
#define _INTEGERVARIABLE_H_

#include "IntegerValue.hpp"
#include "Variable.hpp"

namespace agentxcpp
{

    class IntegerVariable : public Variable<IntegerValue>
    {
        private:

            IntegerValue v;

        public:

            IntegerVariable(IntegerValue _value = IntegerValue())
        {
                v = _value;
        }

            void setValue(IntegerValue _value)
            {
                v = _value;
            }

            virtual IntegerValue get()
            {
                return v;
            }

            virtual IntegerValue value()
            {
                return v;
            }

    };

}

#endif // _INTEGER_H_
