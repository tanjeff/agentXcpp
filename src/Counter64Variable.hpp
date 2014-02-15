#ifndef _COUNTER64VARIABLE_H_
#define _COUNTER64VARIABLE_H_

#include "Counter64Value.hpp"
#include "Variable.hpp"

namespace agentxcpp
{

    class Counter64Variable : public Variable<Counter64Value>
    {
        private:

            Counter64Value v;

        public:

            Counter64Variable(Counter64Value _value = Counter64Value())
        {
                v = _value;
        }

            void setValue(Counter64Value _value)
            {
                v = _value;
            }

            virtual Counter64Value get()
            {
                return v;
            }

            virtual Counter64Value value()
            {
                return v;
            }

    };
}
#endif // _COUNTER64VARIABLE_H_
