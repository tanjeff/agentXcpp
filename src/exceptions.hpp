/*
 * Copyright 2011 Tanjeff-Nicolai Moos <tanjeff@cccmz.de>
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

#ifndef _EXCEPTIONS_H_
#define _EXCEPTIONS_H_

#include <exception>

namespace agentxcpp
{

    /**
     * \brief Exception for parse errors
     */
    class parse_error : public std::exception { };

    /**
     * \brief Exception for invalid parameter.
     */
    class inval_param : public std::exception { };

    /**
     * \brief Exception for reception of %PDU with invalid version number.
     */
    class version_error : public std::exception { };

    /**
     * \brief Exception to indicate a disconnected state
     */
    class disconnected : public std::exception { };

    /**
     * \brief Exception to indicate a timeout
     */
    class timeout_error : public std::exception { };

    /**
     * \brief Exception to indicate a network error
     */
    class network_error : public std::exception { };

    /**
     * \brief Exception to indicate an internal error.
     *
     * This is probably a programming error in the AgentXcpp library.
     */
    class internal_error : public std::exception { };

    /**
     * \brief Exception to indicate that the master agent was unable to process a 
     *        request.
     */
    class master_is_unable : public std::exception { };

    /**
     * \brief Exception to indicate that the master agent was not willing to 
     *        process a request.
     *
     * This could for example happen if the the master agent does not wish to 
     * permit a registration for implementation-specific reasons.
     */
    class master_is_unwilling : public std::exception { };

    /**
     * \brief Exception to indicate that a MIB region was registered twice with 
     *        the same priority.
     *
     * This error can occur if two distinct subagents register the same subtree 
     * with the same priority. It can of course also occur if a single subagent 
     * does a double-registration.
     */
    class duplicate_registration : public std::exception { };

    /**
     * \brief Exception to indicate that a MIB region registration was not
     *        found.
     *
     * This error can occur when unregistering a MIB region which was not 
     * previously registered. It also occurs when adding an SNMP variable with 
     * an OID which does not reside within a registered MIB region.
     */
    class unknown_registration : public std::exception { };

} // namespace agentxcpp

#endif
