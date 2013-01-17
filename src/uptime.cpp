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
#include <boost/date_time/posix_time/posix_time.hpp>

#include "uptime.hpp"

using boost::date_time::microsec_clock;
using boost::posix_time::ptime;
using boost::posix_time::time_duration;


namespace agentxcpp
{
    /**
     * \internal
     *
     * \brief Variable to measure the uptime of the current process.
     *
     * This variable is initialized when the executable starts up
     * and holds the time at which this happened. Afterwards,
     * the uptime of the process can be calculated.
     */
    static ptime process_start_time(microsec_clock<ptime>::universal_time());

    TimeTicks processUpTime()
    {
        // Calculate uptime
        time_duration uptime = microsec_clock<ptime>::universal_time()
                               - process_start_time;

        // Convert uptime to hundreths of seconds
        TimeTicks sysuptime( uptime.total_milliseconds()/10 );

        // Return result
        return sysuptime;
    }

}
