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


// Throws boost::system::system_error:
timeout_timer::timeout_timer(boost::shared_ptr<boost::asio::io_service> io_service)
    : status(standby),
      timer(*io_service)
{
    try
    {
        // Throws boost::system::system_error:
        timer.expires_at(boost::posix_time::pos_infin);
    }
    catch(boost::system::system_error)
    {
        // Timer broke
        status = broken;
    }

    // Start timer
    timer.async_wait(boost::bind(&timeout_timer::check_deadline, this));
}

// Throws boost::system::system_error:
void timeout_timer::expires_at(boost::asio::deadline_timer::time_type time)
{
    if(status == broken)
    {
        // Object is broken -> do nothing
        return;
    }

    status = running;

    try
    {
        // Throws boost::system::system_error:
        timer.expires_at(time);
    }
    catch(boost::system::system_error)
    {
        // Timer broke
        status = broken;
    }
}


// Throws boost::system::system_error:
void timeout_timer::expires_from_now(boost::asio::deadline_timer::duration_type duration)
{
    if(status == broken)
    {
        // Object is broken -> do nothing
        return;
    }

    status = running;

    try
    {
        // Throws boost::system::system_error:
        timer.expires_from_now(duration);
    }
    catch(boost::system::system_error)
    {
        // Timer broke
        status = broken;
    }
}


// Throws boost::system::system_error:
void timeout_timer::check_deadline()
{
    if(status == broken)
    {
        // Object is broken, -> do nothing (not even restart the timer)
        return;
    }

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
        try
        {
            // Throws boost::system::system_error:
            timer.expires_at(boost::posix_time::pos_infin);
        }
        catch(boost::system::system_error)
        {
            // Object broke
            status = broken;

            // No further processing
            return;
        }
    }

    // Re-start timer
    timer.async_wait(boost::bind(&timeout_timer::check_deadline, this));
}


// Throws boost::system::system_error:
void timeout_timer::cancel()
{
    if(status == broken)
    {
        // Object is broken, -> do nothing (not even restart the timer)
        return;
    }

    status = standby;

    try
    {
        // Throws boost::system::system_error:
        timer.expires_at(boost::posix_time::pos_infin);
    }
    catch(boost::system::system_error)
    {
        // Timer broke
        status = broken;
    }
}
