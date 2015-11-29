#ifndef _DISPLAYSTRING_H_
#define _DISPLAYSTRING_H_

// Make the class agentxcpp::Variable available:
#include "AbstractVariable.hpp"
// Make the class agentxcpp::IntegerValue available:
#include "OctetStringVariable.hpp"

namespace agentxcpp
{

/**
 * \brief A DisplayString variable.
 *
 * This class represent an SNMP variable with SYNTAX DisplatString. The data 
 * is stored and processed as an OctetStringVariable, but this class provides 
 * additional methods for easy string handling.
 */
class DisplayStringVariable : public OctetStringVariable
{

    public:

        /**
         * \brief (Default) constructor.
         *
         * Create a DisplayString variable.
         *
         * \param _value The initial value of the variable.
         *
         * \exception None.
         */
        DisplayStringVariable(QString _value = QString())
        {
            OctetStringVariable::setValue(_value);
        }

        /**
         * \brief Set the value of the variable.
         *
         * \param _value The new value.
         *
         * \exception None.
         */
        void setValue(QString _value)
        {
            OctetStringVariable::setValue(_value);
        }

        /**
         * \brief Get the value of the variable.
         *
         * \return The current value.
         *
         * \exception None.
         */
        QString value()
        {
            return OctetStringVariable::toString();
        }

};

}
#endif // _DISPLAYSTRING_H_
