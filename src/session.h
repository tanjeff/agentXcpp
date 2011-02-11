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
