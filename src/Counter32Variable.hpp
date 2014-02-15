#ifndef _COUNTER32VARIABLE_H_
#define _COUNTER32VARIABLE_H_

#include "Counter32Value.hpp"
#include "Variable.hpp"

namespace agentxcpp
{

    class Counter32Variable : public Variable<Counter32Value>
    {
        private:

            Counter32Value v;

        public:

            Counter32Variable(Counter32Value _value = Counter32Value())
        {
                v = _value;
        }

            void setValue(Counter32Value _value)
            {
                v = _value;
            }

            virtual Counter32Value get()
            {
                return v;
            }

            virtual Counter32Value value()
            {
                return v;
            }

    };
}
#endif // _COUNTER32VARIABLE_H_
