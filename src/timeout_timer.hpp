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
#ifndef _TIMEOUT_TIMER_H_
#define _TIMEOUT_TIMER_H_


#include <boost/asio.hpp>

namespace agentxcpp
{
    /**
     * \internal
     *
     * \brief A timer which can detect timeouts
     *
     * This timer can detect timeouts. A timeout_timer object starts in 
     * "standby" state. The timer can then be started using the expires_at() 
     * (giving an absolute time) or expires_from_now() (giving a time span) 
     * methods, which changes the status to "running". When the timer expires, 
     * the status changes to "expired". The timer can be disabled at any time 
     * using cancel(), which changes the status back to "standby" (this works 
     * also with an expired timer).
     *
     * This class uses the boost::asio library and therefore needs an 
     * io_service object. It never calls io_service::run() or 
     * io_service::run_one(). For the timeout_timer to work, the run() or 
     * run_one() method of the io_service object must be invoked by the user of 
     * timeout_timer.
     * 
     * The object can break, e.g. when something with the interally used 
     * deadline_timer goes wrong. In this case the status changes to "broken".  
     * A broken timeout_timer cannot be used anymore: a call to any member 
     * function will do nothing (except for get_status(), which will report the 
     * status "broken") and the status will be "broken" forever.
     *
     * \par How it works
     *
     * For timeout detection, a boost deadline timer is used together with the 
     * status member. While the timer is not in use, it's expiry time is set to 
     * "infinite" and status is set to "standby". When starting operations 
     * which need timeout detection, the deadline timer's expiry is set to the 
     * according time, and status is set to "running". If the timer expires, 
     * the callback function check_deadline() is invoked, which sets the expiry 
     * back to "infinite" and status to "expired". If the operation completes 
     * before the timer expires, cancel() can be invoked to set the expiry back 
     * to "infinite" and the status to "standby".
     *
     * Note that the callback check_deadline() may be invoked errornously, e.g.  
     * in this situation:
     * -# The monitored operation completes
     * -# The timer expires in the backround and check_deadline() is scheduled 
     *  for invokation at the next call to io_service::run()
     * -# The cancel() function is invoked
     * The monitored operation thus completed without a timeout condition, but 
     * check_deadline() will nevertheless be invoked later. Therefore, it 
     * compares the expiry date of the timer with the current timestamp to 
     * determine whether the timer really expired. 
     */
    class timeout_timer
    {
        public:

            /**
             * \brief Constructor
             *
             * This constructor sets the timer status to "standby".
             *
             * \note If something goes wrong, the status is set to "broken".
             *
             * \param io_service The io_service object which will be used by
             *                   the timer.
             *
             * \exception None.
             */
            timeout_timer(boost::asio::io_service & io_service);

            /**
             * \brief The timeout status type.
             */
            enum status_t
            {
                running,     // timer is running
                expired,     // timer expired
                standby,     // timer currently not in use
                broken
            };

            /**
             * \brief Time out at the given time.
             *
             * This function starts the timer and set its expiry time to 
             * 'time'. The status is set to 'running'.
             *
             * \note If something goes wrong, the status is set to "broken".
             *
             * \param time The time at which the timer shall expire.
             *
             * \exception None.
             */
            void expires_at(boost::asio::deadline_timer::time_type time);

            /**
             * \brief Time out at (now + duration).
             *
             * This function starts the timer and set its expiry time to (now + 
             * 'duration'). The status is set to 'running'.
             *
             * \note If something goes wrong, the status is set to "broken".
             *
             * \param duration The duration after which the timer elapses.
             *
             * \exception None.
             */
            void expires_from_now(boost::asio::deadline_timer::duration_type duration);

            /**
             * \brief Stop the timer.
             *
             * This stops the timer. The status is set to "standby".
             *
             * \note If something goes wrong, the status is set to "broken".
             *
             * \exception None.
             */
            void cancel();

            /**
             * \brief Get the current timer status.
             *
             * \exception None.
             */
            status_t get_status()
            {
                return status;
            }

        private:

            /**
             * \brief Hide default constructor.
             *
             * We need an io_service object to function properly.
             */
            timeout_timer();

            /**
             * \brief The timeout status.
             */
            status_t status;
    
            /**
             * \brief The boost::asio timer.
             *
             * This timer is used to detect timeout conditions.
             */
            boost::asio::deadline_timer timer;

            /**
             * \brief The timeout handler.
             *
             * This function is used as callback handler for 'timer'. It 
             * compares the timer's expiry time with the current system time to 
             * detect errornous invokation (in case cancel() set the expiry to 
             * "infinite" before check_deadline() was invoked). If a real 
             * timeout is detected, the status is set to "expired" and the 
             * expiry is set to "infinite".
             *
             * This callback also restarts the timer on each call to keep it 
             * running.
             *
             * If something goes wrong, the status is set to "broken".
             */
            void check_deadline();

    };

}

#endif
