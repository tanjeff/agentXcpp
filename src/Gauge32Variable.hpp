#ifndef _GAUGE32VARIABLE_H_
#define _GAUGE32VARIABLE_H_

#include "Gauge32Value.hpp"
#include "Variable.hpp"

namespace agentxcpp
{

    class Gauge32Variable : public Variable<Gauge32Value>
    {
        private:

            Gauge32Value v;

        public:

            Gauge32Variable(Gauge32Value _value = Gauge32Value())
        {
                v = _value;
        }

            void setValue(Gauge32Value _value)
            {
                v = _value;
            }

            virtual Gauge32Value get()
            {
                return v;
            }

            virtual Gauge32Value value()
            {
                return v;
            }

    };
}
#endif // _GAUGE32VARIABLE_H_
