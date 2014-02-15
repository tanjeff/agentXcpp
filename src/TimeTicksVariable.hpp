#ifndef _TIMETICKSVARIABLE_H_
#define _TIMETICKSVARIABLE_H_

#include "TimeTicksValue.hpp"
#include "Variable.hpp"

namespace agentxcpp
{

    class TimeTicksVariable : public Variable<TimeTicksValue>
    {
        private:

            TimeTicksValue v;

        public:

            TimeTicksVariable(TimeTicksValue _value = TimeTicksValue())
        {
                v = _value;
        }

            void setValue(TimeTicksValue _value)
            {
                v = _value;
            }

            virtual TimeTicksValue get()
            {
                return v;
            }

            virtual TimeTicksValue value()
            {
                return v;
            }

    };
}
#endif // _TIMETICKSVARIABLE_H_
