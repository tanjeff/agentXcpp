/*
 * Copyright 2011-2012 Tanjeff-Nicolai Moos <tanjeff@cccmz.de>
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

#include <boost/bind.hpp>

#include "timeout_timer.hpp"


using namespace agentxcpp;


timeout_timer::timeout_timer(boost::shared_ptr<boost::asio::io_service> io_service)
    : status(standby),
      timer(*io_service)
{
        timer.expires_at(boost::posix_time::pos_infin);
}

void timeout_timer::expires_at(boost::asio::deadline_timer::time_type time)
{
    status = running;
    timer.expires_at(time);
}


void timeout_timer::expires_from_now(boost::asio::deadline_timer::duration_type duration)
{
    status = running;
    timer.expires_from_now(duration);
}


void timeout_timer::check_deadline()
{
    // Check whether the deadline has passed. We compare the deadline against 
    // the current time since a new asynchronous operation may have moved the
    // deadline before this callback had a chance to run.
    if (timer.expires_at() <= boost::asio::deadline_timer::traits_type::now())
    {
        // The deadline has passed.
        // -> set status to "expired"
        status = expired;

        // There is no longer an active deadline. The expiry is set to positive
        // infinity so that the callback is not invoked until a new deadline is 
        // set.
        timer.expires_at(boost::posix_time::pos_infin);
    }

    // Re-start timer
    timer.async_wait(boost::bind(&timeout_timer::check_deadline, this));
}


void timeout_timer::stop()
{
    status = standby;
    timer.expires_at(boost::posix_time::pos_infin);
}
