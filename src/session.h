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
