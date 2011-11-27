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

#ifndef __EXCEPTIONS_H__
#define __EXCEPTIONS_H__

/**
 * \brief Exception for parse errors
 */
class parse_error { };

/**
 * \brief Exception for invalid parameter.
 */
class inval_param { };

/**
 * \brief Exception is thrown if the master has sent a data packet with a wrong
 *        AgentX protocol version
 */
class version_mismatch { };

/**
 * \brief Exception to indicate a disconnected state
 */
class disconnected { };

/**
 * \brief Exception to indicate a timeout
 */
class timeout_exception { };

/**
 * \brief Exception to indicate a network error
 */
class network { };

/**
 * \brief Exception to indicate an internal error.
 *
 * This is probably a programming error in the AgentXcpp library.
 */
class internal_error { };

/**
 * \brief Exception to indicate that the master agent was unable to process a 
 * request.
 */
class master_is_unable { };

/**
 * \brief Exception to indicate that the master agent was not willing to 
 * process a request.
 *
 * This could for example happen if the the master agent does not wish to 
 * permit a registration for implementation-specific reasons.
 */
class master_is_unwilling { };

/**
 * \brief Exception to indicate that a MIB region was registered twice with 
 * the same priority.
 *
 * This error can occur if two distinct subagents register the same subtree 
 * with the same priority. It can of course also occur if a single subagent 
 * does a double-registration.
 */
class duplicate_registration { };

#endif
