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

#include <boost/asio.hpp>

#include <fstream>
#include <string>
#include "types.hpp"
#include "oid.hpp"
#include "ClosePDU.hpp"

namespace agentxcpp
{
    /**
     * \brief This class represents the master agent in a subagent program.
     *
     * This class is used on the subagent's side of a connection between 
     * subagent and master agent. It serves as a proxy which represents the 
     * master agent. It is possible for a subagent to hold connections to more 
     * than one master agents. For each connection one master_proxy object is 
     * created. Multiple connections to the same master agent are possible, too 
     * (although this is probably not useful), in which case one master_proxy 
     * per connection is needed.
     *
     * The master_proxy is always in one of the following states:
     *
     * -# connected
     * -# disconnected
     *
     * The session to the master agent is established when creating a 
     * master_proxy object, thus the object usually starts in connected state.  
     * If that fails, the object will start in disconnected state. It is 
     * possible to re-connect with the reconnect() function at any time (even 
     * if the session is currently established - it will be shut down and 
     * re-established in this case).  When the object is destroyed, the session 
     * will be cleanly shut down. The connections state can be inspected with 
     * the is_connected() function. Some functions throw a disconnected 
     * exception if the session is not currently established.
     *
     */
    // TODO: describe timeout handling
    // TODO: byte ordering is constant for a session. See rfc 2741, 7.1.1
    class master_proxy
    {
	private:
	    /**
	     * \brief The socket.
	     */
	    boost::asio::local::stream_protocol::socket socket;

	    /**
	     * \brief The endpoint used fo unix domain sockets.
	     */
	    boost::asio::local::stream_protocol::endpoint endpoint;
	    
	    /**
	     * \brief The mandatory io_service object.
	     *
	     * This object is needed for boost::asio sockets.
	     */
	    boost::asio::io_service io_service;

	    /**
	     * \brief The session ID of the current session.
	     *
	     * If disconnected, the value is undefined.
	     */
	    uint32_t sessionID;

	    /**
	     * \brief A string describing the subagent.
	     *
	     * Set upon object creation.
	     */
	    std::string description;

	    /**
	     * \brief Default timeout of the session (in seconds).
	     *
	     * A value of 0 indicates that there is no session-wide default.
	     */
	    byte_t default_timeout;

	    /**
	     * \brief An Object Identifier that identifies the subagent. May be
	     *        the null OID.
	     */
	    oid id;

	public:
	    /**
	     * \brief Create a session object connected via unix domain
	     *        socket
	     *
	     * The constructor tries to connect to the master agent. If that 
	     * fails, the object is created nevertheless and will be in state 
	     * disconnected.
	     *
	     * \param description A string describing the subagent. This
	     *                    description cannot be changed later.
	     *
	     * \param default_timeout The length of time, in seconds, that the
	     *                        master agent should allow to elapse after 
	     *                        receiving a message before it regards the 
	     *                        subagent as not responding. Allowed 
	     *                        values are 0-255, with 0 meaning "no 
	     *                        default for this session".
	     *
	     * \param ID An Object Identifier that identifies the subagent.
	     *           Default is the null OID (no ID).
	     *
	     * \param unix_domain_socket The socket file to connect to.
	     *                           Defaults to /var/agentx/master, as 
	     *                           desribed in RFC 2741, section 8.2.1 
	     *                           "Well-known Values".
	     */
	    master_proxy(std::string description="",
		   byte_t default_timeout=0,
		   oid ID=oid(),
		   std::string unix_domain_socket="/var/agentx/master");

	    /**
	     * \brief Check whether the session is in state connected
	     *
	     * \returns true if the session is connected, false otherwise.
	     */
	    bool is_connected()
	    {
		return socket.is_open();
	    }

	    /**
	     * \brief Connect to the master agent.
	     *
	     * Note that upon creation of a session object, the connection is 
	     * automatically established. If the current state is "connected", 
	     * the function does nothing.
	     */
	    void connect();

	    /**
	     * \brief Shutdown the session.
	     *
	     * Disconnect from the master agent. Note that upon destruction of 
	     * a session object the session is automatically shutdown. If the 
	     * session is in state "disconnected", the function does nothing.
	     *
	     * \param reason The shutdown reason is reported to the master
	     *               agent during shutdown.
	     */
	    void disconnect(ClosePDU::reason_t reason=ClosePDU::reasonShutdown);

	    /**
	     * \brief Reconnect to the master agent.
	     *
	     * Disconnects, then connects to the master agent. If the status is 
	     * diconnected, the master is connected.
	     */
	    void reconnect();

	    /**
	     * \brief Get the sessionID of the session
	     *
	     * Get the session ID of the last established session, even if the 
	     * current state is "disconnected".
	     *
	     * \return The session ID of the last established session. If
	     *         object was never connected to the master, 0 is 
	     *         returned.
	     */
	    uint32_t get_sessionID()
	    {
		return sessionID;
	    }
    
	    /**
	     * \brief Default destructor
	     *
	     * The default destructor cleanly shuts down the session (if it is 
	     * currently established) and destroys the session object.
	     */
	    ~master_proxy();

    };
}

#endif
