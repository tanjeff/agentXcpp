/*
 * Copyright 2011 Tanjeff-Nicolai Moos <tanjeff@cccmz.de>
 *
 * This file is part of agentXcpp.
 *
 * AgentXcpp is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * AgentXcpp is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with AgentXcpp.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __AGENTX_H__
#define __AGENTX_H__

#include <fstream>
#include <string>
#include <boost/asio.hpp>
#include "types.h"

namespace agentx
{
    class session
    {
	private:
	    boost::asio::local::stream_protocol::endpoint endpoint;
	    
	    boost::asio::io_service io_service;
	    boost::asio::local::stream_protocol::socket socket;

	    uint32_t sessionID;

	public:
	    /**
	     * \brief Create an agentx object connected via unix domain socket
	     */
	    session(std::string unix_domain_socket);

	    ~session();

    };
}

#endif
