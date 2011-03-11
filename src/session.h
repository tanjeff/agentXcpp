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
#ifndef __AGENTX_H__
#define __AGENTX_H__

#include <fstream>
#include <string>
#include <boost/asio.hpp>
#include "types.h"

namespace agentx
{
    /**
     * \brief This class represents the subagent side of an agentX session.
     *
     * This class represents the subagent's side of a session between subagent 
     * and master agent.  It is possible for a subagent to hold multiple 
     * sessions, even to different master agents (although this is probably not 
     * useful).
     *
     * A session is always in one of two states:
     * 
     * -# connected
     * -# disconnected
     * 
     * The session to the master agent is established when creating a session 
     * object, thus the object usually starts in connected state. If that 
     * fails, the object starts in disconnected state. It is possible to 
     * re-connect with the reconnect() function at any time (even if the 
     * session is currently established - it will be shut down and 
     * re-established in this case). When the object is destroyed, the session 
     * will be cleanly shut down.
     *
     * The connections state can be inspected with the is_connected() function.      
     *
     * Some member functions throw a disconnected exception if the session is 
     * not currently established.
     */
    class session
    {
	private:
	    /**
	     * \brief The endpoint used fo unix domain sockets.
	     */
	    boost::asio::local::stream_protocol::endpoint endpoint;
	    
	    boost::asio::io_service io_service;
	    boost::asio::local::stream_protocol::socket socket;

	    /**
	     * \brief The session ID of the current session.
	     *
	     * If disconnected, the value is undefined.
	     */
	    uint32_t sessionID;

	    /**
	     * \brief The last known connect status.
	     *
	     * This member is set to true when a connection is established. It 
	     * is set to false when a disconnect status is detected.
	     *
	     * \warning This member may still be true while the connection was
	     *          already lost. This can happen if the connection loss 
	     *          was not yet detected.
	     */
	    bool connected;

	public:
	    /**
	     * \brief Create an agentx object connected via unix domain socket
	     */
	    session(std::string unix_domain_socket);

	    /**
	     * \brief Check whether the connection to the master is alive
	     *
	     * This function sends a ping PDU to check whether the connection 
	     * to the master is functional.
	     *
	     * \returns true if the master responds to the ping request, false
	     *          otherwise
	     */
	    bool is_connected();

	    /**
	     * \brief Connect to the master agent.
	     *
	     * Note that upon creation of a session object, the connection is 
	     * automatically established.
	     *
	     * If the connection is already established, this function does 
	     * nothing.
	     *
	     * \internal
	     *
	     * Sets connected to true.
	     */
	    void connect();

	    /**
	     * \brief Shutdown the session.
	     *
	     * Disconnect from the master agent. Note that upon destruction of 
	     * a session object the session is automatically shutdown.
	     */
	    void disconnect();

	    /**
	     * \brief Reconnect to the master agent.
	     *
	     */
	    void reconnect();

	    /**
	     * \brief Get the sessionID of the session
	     *
	     * Get the session ID of the last established session, even if the 
	     * currently not connected to the master.
	     *
	     * \return The session ID of the last established session. If
	     *         object was never connected to the master, returns 0.
	     */
	    uint32_t get_sessionID()
	    {
		return sessionID;
	    }

	    ~session();

    };
}

#endif
