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
#ifndef _UPTIME_HPP_
#define _UPTIME_HPP_

#include "TimeTicks.hpp"

namespace agentxcpp
{
    /**
     * \brief Calculate the uptime of the current process.
     *
     * This function calculates the uptime of the current process, in
     * hundreths of a second. The result may be given as sysUpTime.0.
     * parameter to \ref master_proxy::send_notification(
     * const boost::optional<TimeTicks>&,
     * const oid&, const vector<varbind>&).
     *
     * \internal
     * The time is measured using a global variable which is
     * initialized to the current time just be before main() starts.
     * \endinternal
     *
     * \return The current uptime, in hundreths of a second.
     */
    TimeTicks processUpTime();

} // namespace agentxcpp

#endif // _UPTIME_HPP_
