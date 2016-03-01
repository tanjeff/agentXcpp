/*
 * Copyright 2011-2016 Tanjeff-Nicolai Moos <tanjeff@cccmz.de>
 *
 * This file is part of the agentXcpp library.
 *
 * AgentXcpp is free software: you can redistribute it and/or modify
 * it under the terms of the AgentXcpp library license, version 1, which 
 * consists of the GNU General Public License and some additional 
 * permissions.
 *
 * AgentXcpp is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * See the AgentXcpp library license in the LICENSE file of this package 
 * for more details.
 */
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
