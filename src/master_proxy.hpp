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
#ifndef _MASTER_PROXY_H_
#define _MASTER_PROXY_H_

#include <string>
#include <map>
#include <list>

#include <boost/shared_ptr.hpp>
#include <boost/cstdint.hpp>
#include <boost/optional/optional.hpp>

#include <QObject>
#include <QThread>

#include "oid.hpp"
#include "variable.hpp"
#include "TimeTicks.hpp"
#include "ClosePDU.hpp"
#include "ResponsePDU.hpp"
#include "RegisterPDU.hpp"
#include "UnregisterPDU.hpp"
#include "GetPDU.hpp"
#include "GetNextPDU.hpp"
#include "TestSetPDU.hpp"
#include "CleanupSetPDU.hpp"
#include "CommitSetPDU.hpp"
#include "UndoSetPDU.hpp"
#include "UnixDomainConnector.hpp"

using boost::uint8_t;
using boost::uint32_t;

namespace agentxcpp
{
    /**
     * \brief This class represents the master agent in a subagent program.
     *
     * \par Introduction
     *
     * This class is used on the subagent's side of a connection between 
     * subagent and master agent. It serves as a proxy which represents the 
     * master agent. It is possible for a subagent to hold connections to more 
     * than one master agents. For each connection one master_proxy object is 
     * created. Multiple connections to the same master agent are possible, 
     * too, in which case one master_proxy per connection is needed.
     */
    /**
     * \par Connection State
     *
     * The master_proxy is always in one of the following states:
     * - connected
     * - disconnected
     * .
     * The session to the master agent is established when creating a 
     * master_proxy object, thus the object usually starts in connected state.  
     * If that fails, the object starts in disconnected state. A connected 
     * master_proxy object may also loose the connection to the master agent 
     * and consequently become disconnected, without informing the user. It is 
     * possible to re-connect with the reconnect() function at any time (even 
     * if the session is currently established - it will be shut down and 
     * re-established in this case).  When the object is destroyed, the session 
     * will be cleanly shut down. The connection state can be inspected with 
     * the is_connected() function.  Some functions throw a disconnected 
     * exception if the session is not currently established.
     *
     */
    /**
     * \par The QApplication object
     *
     * The agentXcpp library uses the QT library. Processing SNMP requests 
     * (such as get and set) is performed within the QApplication event loop. 
     * Therefore, a program using agentXcpp must create a QApplication object 
     * and call exec() on it to start the event loop. The agentXcpp library 
     * also creates a QThread object for the networking part. Networking is 
     * done within the event loop of that thread.
     *
     */
    /**
     * \par Registrations
     *
     * Before the master agent sends requests to a subagent, the subagent must 
     * register a subtree. Doing so informs the master agent that the subagent 
     * wishes to handle requests for these OIDs. A subtree is an OID which 
     * denotes the root of a subtree in which the offered objects
     * resides. For example, when two objects shall be offered with the OIDs 
     * 1.3.6.1.4.1.42<b>.1.1</b> and 1.3.6.1.4.1.42<b>.1.2</b>, then a subtree 
     * with OID 1.3.6.1.4.1.42<b>.1</b> should be registered, which includes 
     * both objects.  The master agent will then forward all requests 
     * conecerning objects in this subtree to this subagent. Requests to 
     * non-existing objects (e.g.  1.3.6.1.4.1.42<b>.1.3</b>) are also 
     * forwarded, and the agentXcpp library will take care of them and return 
     * an appropriate error to the master agent.
     * 
     * The function register_subtree() is used to register a subtree. It is 
     * typically called for the highest-level OID of the MIB which is 
     * implemented by the subagent. However, it is entirely possible to 
     * register multiple subtrees.
     *
     * Identical subtrees are subtrees with the exact same root OID. Each 
     * registration is done with a priority value.  The higher the value, the 
     * lower the priority.  When identical subtrees are registered (by the same 
     * subagent or by different subagents), the priority value is used to 
     * decide which subagent gets the requests.  The master refuses identical 
     * registrations with the same priority values.  Note however, that in case 
     * of overlapping subtrees which are \e not identical (e.g.  
     * 1.3.6.1.4.1.42<b>.1</b> and 1.3.6.1.4.1.42<b>.1.33.1</b>), the most 
     * specific subtree (i.e. the one with the longest OID) wins regardless of 
     * the priority values.
     *
     * It is also possible to unregister subtrees using the 
     * unregister_subtree() function. This informs the master agent that the 
     * MIB region is no longer available from this subagent.
     *
     * \internal
     *
     * The master_proxy object generates a RegisterPDU object each time a 
     * registration is performed. These RegisterPDU objects are stored in the 
     * registrations member.
     *
     * When unregistering, the matching RegisterPDU is removed from the 
     * registration member.
     *
     * The registration member becomes invalid on connection loss. Since a 
     * connection loss is not signalled, the member cannot be cleared in such 
     * situations. Therefore, it is cleared in the connect() method if the 
     * object is currently disconnected. If connect() is called on a connected 
     * master_proxy object, the registrations member is not cleared.
     *
     * \endinternal
     */
    /**
     * \par Adding and Removing Variables
     *
     * Registering a subtree does not make any SNMP variables accessible yet.  
     * To provide SNMP variables, they must be added to the master_proxy 
     * object, e.g. using add_variable(). The master_proxy object will then 
     * dispatch incoming requests to the variables it knows about. If a request 
     * is received for an OID for which no variable has been added, an 
     * appropriate error is returned to the master agent.
     *
     * Added variables can be removed again using remove_variable(). This makes 
     * a variable inaccessible for the master agent.
     *
     * \internal
     *
     * The variables are stored in the member variables, which is a 
     * std::map<oid, shared_ptr<variable> >. The key is the OID for which the 
     * variable was added. This allows easy lookup for the request 
     * dispatcher.
     *
     * When removing a variable, it is removed from the variables member.
     *
     * The variables member becomes invalid on connection loss. Since a 
     * connection loss is not signalled, the member cannot be cleared in such 
     * situations.  Therefore, it is cleared in the connect() method if the 
     * object is currently disconnected. If connect() is called on a connected 
     * master_proxy object, the variables member is not cleared.
     *
     * \endinternal
     *
     */
    /**
     * \internal
     *
     * \par Internals
     * 
     * Receiving and processing PDU's coming from the master is done using the 
     * UnixDomainConnector class. The master_proxy implements the handle_pdu() 
     * slot, which is connected to the UnixDomainConnector::pduArrived() 
     * signal.
     *
     * \endinternal
     *
     * \todo Describe timeout handling
     * \todo Byte ordering is constant for a session. See rfc 2741, 7.1.1
     */
    class master_proxy : public QObject
    {
        Q_OBJECT

	private:

            /**
             * \brief The thread running a UnixDomainConnector.
             *
             * The UnixDomainConnector object is moved into this thread after 
             * creation.
             */
            QThread m_thread;

            /**
	     * \brief The path to the unix domain socket.
	     */
	    std::string socket_file;

	    /**
	     * \brief The connector object used for networking.
	     *
	     * Created by constructors, destroyed by destructor.
	     */
	    UnixDomainConnector* connection;

	    /**
	     * \brief The session ID of the current session.
	     *
	     * If disconnected, the value is undefined.
	     */
	    uint32_t sessionID;

	    /**
	     * \brief A string describing the subagent.
	     *
	     * Set upon object creation. It is allowed to be emtpy.
	     */
	    std::string description;

	    /**
	     * \brief Default timeout of the session (in seconds).
	     *
	     * A value of 0 indicates that there is no session-wide default.
	     */
	    uint8_t default_timeout;

	    /**
	     * \brief An Object Identifier that identifies the subagent. May be
	     *        the null OID.
	     */
	    oid id;

	    /**
	     * \brief The registrations.
	     *
             * Every time an registration is performed, the RegisterPDU is 
             * stored in this list. This allows to automatically re-register 
             * these subtrees on reconnect.
	     */
	    std::list< boost::shared_ptr<RegisterPDU> > registrations;

	    /**
	     * \brief Storage for all SNMP variables known to the master_proxy.
	     */
	    std::map< oid, shared_ptr<variable> > variables;

            /**
             * \brief The variables affected by the Set operation currently
             *        in progress.
             *
             * The CommitSet, UndoSet and CleanupSet PDU's do not contain the 
             * affected variables but operate on the variables denominated in 
             * the preceding TestSet PDU.
             *
             * These are the variables denominated in that TestSet PDU.
             */
            std::list< shared_ptr<variable> > setlist;

	    /**
	     * \brief Send a RegisterPDU to the master agent.
	     *
             * This function sends a RegisterPDU to the master agent, waits for 
             * the response and evaluates it.
             *
	     * \param pdu The RegisterPDU to send.
	     *
	     * \exception disconnected If the master_proxy is currently in
	     *                         state 'disconnected'.
	     *
	     * \exception timeout_error If the master agent does not
	     *                          respond within the timeout interval.
	     *
	     * \exception internal_error If the master received a malformed
             *                           %PDU. This is probably a programming 
             *                           error within the agentXcpp library.
	     *
	     * \exception master_is_unable The master agent was unable to
	     *                             perform the desired register 
	     *                             request.  The reason for that is 
	     *                             unknown.
	     *
	     * \exception duplicate_registration If the exact same subtree was
             *                                   already registered, either by 
             *                                   another subagent or by this 
             *                                   subagent.
	     *
	     * \exception master_is_unwilling If the master was unwilling for
	     *                                some reason to make the desired 
	     *                                registration.
	     *
	     * \exception parse_error If an unexpected response was received
	     *                        from the master. This is probably a 
	     *                        programming error within the master 
	     *                        agent.  It is possible that the master 
	     *                        actually performed the desired 
	     *                        registration and that a retry will result 
	     *                        in a duplicate_registration error.
	     */
	    void do_registration(boost::shared_ptr<RegisterPDU> pdu);

	    /**
	     * \brief Send a UnregisterPDU to the master agent.
	     *
             * This function sends an UnregisterPDU to the master agent which 
             * revokes the registration done by a RegisterPDU.  Then it waits 
             * for the response and evaluates it.
             *
             * \param pdu The UnregisterPDU to send.
	     *
	     * \exception disconnected If the master_proxy is currently in
	     *                         state 'disconnected'.
	     *
	     * \exception timeout_error If the master agent does not
	     *                          respond within the timeout interval.
	     *
	     * \exception internal_error If the master received a malformed
	     *                           PDU. This is probably a programming 
	     *                           error within the agentXcpp library.
	     *
	     * \exception master_is_unable The master agent was unable to
	     *                             perform the desired register 
	     *                             request.  The reason for that is 
	     *                             unknown.
	     *
	     * \exception unknown_registration The MIB region is not currently
	     *                                 registered with this parameters.
	     *
	     * \exception parse_error If an unexpected response was received
	     *                        from the master. This is probably a 
	     *                        programming error within the master 
	     *                        agent.  It is possible that the master 
	     *                        actually performed the desired 
	     *                        registration and that a retry will result 
	     *                        in a duplicate_registration error.
	     */
	    void undo_registration(boost::shared_ptr<UnregisterPDU> pdu);

	   /**
	    * \brief Create UnregisterPDU for undoing a registration.
            *
            * This function creates an UnregisterPDU which unregisters the MIB 
            * region which was registered with the given RegisterPDU.
            *
            * \param pdu The RegisterPDU which was used for registration.
	    *
            * \return The created UnregisterPDU.
	    *
	    * \exception None.
            */
	    boost::shared_ptr<UnregisterPDU> create_unregister_pdu(
				    boost::shared_ptr<RegisterPDU> pdu);

            /**
             * \brief Handle incoming GetPDU's.
             *
             * This method is called by handle_pdu(). It processes the given 
             * GetPDU and stores the results in the given ResponsePDU (i.e. it 
             * adds varbinds to the ResponsePDU).
             *
             * \param response The pre-initialized ResponsePDU. Varbinds are
             *                 added to this PDU during processing.
             *
             * \param get_pdu The GetPDU to be processed.
             */
            void handle_getpdu(shared_ptr<ResponsePDU> response, shared_ptr<GetPDU> get_pdu);

            /**
             * \brief Handle incoming GetNextPDU's.
             *
             * This method is called by handle_pdu(). It processes the given 
             * GetNextPDU and stores the results in the given ResponsePDU (i.e.  
             * it adds Varbinds to the ResponsePDU).
             *
             * \param response The pre-initialized ResponsePDU. Varbinds are
             *                 added to this PDU during processing.
             *
             * \param getnext_pdu The GetNextPDU to be processed.
             */
            void handle_getnextpdu(shared_ptr<ResponsePDU> response, shared_ptr<GetNextPDU> getnext_pdu);

            /**
             * \brief Handle incoming TestSetPDU's.
             *
             * This method is called by handle_pdu(). It processes the given 
             * TestSetPDU and stores the results in the given ResponsePDU.
             *
             * \param response The pre-initialized ResponsePDU.
             *
             * \param testset_pdu The TestSetPDU to be processed.
             */
            void handle_testsetpdu(boost::shared_ptr<ResponsePDU> response, shared_ptr<TestSetPDU> testset_pdu);

            /**
             * \brief Handle incoming CleanupSetPDU's.
             *
             * This method is called by handle_pdu(). It calls the CleanupSet 
             * handler for each variable in 'setlist'.
             */
            void handle_cleanupsetpdu();

            /**
             * \brief Handle incoming CommitSetPDU's.
             *
             * This method is called by handle_pdu(). It processes the given 
             * CommitSetPDU and stores the results in the given ResponsePDU.
             *
             * \param response The pre-initialized ResponsePDU.
             *
             * \param commitset_pdu The CommitSetPDU to be processed.
             */
            void handle_commitsetpdu(boost::shared_ptr<ResponsePDU> response, shared_ptr<CommitSetPDU> commitset_pdu);

            /**
             * \brief Handle incoming UndoSetPDU's.
             *
             * This method is called by handle_pdu(). It processes the given
             * UndoSetPDU and stores the results in the given ResponsePDU.
             *
             * \param response The pre-initialized ResponsePDU.
             *
             * \param undoset_pdu The UndoSetPDU to be processed.
             */
            void handle_undosetpdu(boost::shared_ptr<ResponsePDU> response, shared_ptr<UndoSetPDU> undoset_pdu);

	public slots:
	    /**
             * \internal
             *
	     * \brief The dispatcher for incoming %PDU's.
	     *
             * This slot is connected to UnixDomainConnector::pduArrived() and 
             * thus called for each incoming PDU (except ResponsePDU's).
             *
             * This method performs the steps described in RFC 2741, 7.2.2.  
             * "Subagent Processing" (except for the steps necessary for 
             * ResponsePDU handling) and calls the methods handle_*() to handle 
             * the individual PDU types. A ResponsePDU is created here and 
             * given to the specialized methods to add their results. Finally 
             * handle_pdu() sends the response.
	     */
	    virtual void handle_pdu(shared_ptr<PDU>);

	    /**
	     * \brief Send a notification or trap.
	     *
	     * This function sends a notification to the master agent, which in
	     * turn sends an SNMP notification or trap, depending on its
	     * configuration.
	     *
	     * Each notification contains the sysUpTime.0 object (optional)
	     * and the snmpTrapOID.0 object (mandatory). The values of both
	     * objects are given as parameters.
	     *
	     * \param sysUpTime The value of the sysUpTime.0 object according
	     *                  to RFC 1907, which says: "<em>The time (in
	     *                  hundredths of a second) since the network
	     *                  management portion of the system was last
	     *                  re-initialized.</em>" This parameter is
	     *                  optional.  You can use You can use \ref
	     *                  agentxcpp::processUpTime() to get the uptime of
	     *                  the current process. If the parameter is not
	     *                  provided, the sysUpTime.0 will not be included
	     *                  in the notification, and the master agent will
	     *                  insert an sysUpTime.0 value (e.g. the uptime of
	     *                  the OS, depending on the master agent).
	     *
	     * \param snmpTrapOID The value of  snmpTrapOID.0 according to
	     *                    RFC 1907, which says: "<em>The authoritative
	     *                    identification of the notification currently
	     *                    being sent.</em>" This is normally the Trap OID as
	     *                    specified in the corresponding MIB.  However,
	     *                    if the notification shall be converted to an SNMPv1
	     *                    trap (this conversion is done by the master agent),
	     *                    the snmpTrapOID.0 value must
	     *                    meet certain requirements. You can use
	     *                    generate_v1_snmpTrapOID()
	     *                    to construct a valid value in that case.
	     *
	     * \param varbinds Additional varbinds which are included in the
	     *                 notification.
	     *
	     * \exception timeout_error FIXME
	     *
	     * \exception disconnected FIXME
	     *
	     * \exception master_is_unable FIXME
	     *
	     * \exception unsupported_context FIXME
	     *
	     * \todo Document exceptions.
	     */
	    void send_notification(const boost::optional<TimeTicks>& sysUpTime,
	                           const oid& snmpTrapOID,
	                           const std::vector<varbind>& varbinds=vector<varbind>());

	    /**
	     * \brief Writing aid: Send notification without sysUpTime.0.
	     *
	     * This calls \ref send_notification(
	     * const boost::optional<TimeTicks>&,
	     * const oid&, const vector<varbind>&) with an empty sysUpTime.0
	     * parameter. Without the writing aid it would be necessary to
	     * construct an empty parameter, like so:
	     * \code
	     * master.send_notification(optional<TimeTicks>(),
	     *                          mySubagentOid);
	     * \endcode
	     *
	     * For the documentation of the parameters and exceptions go to
	     * \ref send_notification(
	     * const boost::optional<TimeTicks>&,
	     * const oid&, const vector<varbind>&)
	     */
	    void send_notification(const oid& snmpTrapOID,
	                           const std::vector<varbind>& varbinds=vector<varbind>())
	    {
	        send_notification(boost::optional<TimeTicks>(),
	                snmpTrapOID,
	                varbinds);
	    }

	public:

            /**
	     * \brief Create a session object connected via unix domain
	     *        socket.
	     *
	     * This constructor tries to connect to the master agent. If that 
	     * fails, the object is created nevertheless and will be in state 
	     * disconnected.
	     *
             * \param description A string describing the subagent. This
	     *                    description cannot be changed later.
	     *
             * \param default_timeout The length of time, in seconds, that
             *                        the master agent should allow to elapse 
             *                        before it regards the subagent as not 
             *                        responding.  The value is also used when 
             *                        waiting synchronously for data from the 
             *                        master agent (e.g. when registering 
             *                        stuff).  Allowed values are 0-255, with 0 
             *                        meaning "no default for this session".
	     *
	     * \param ID An Object Identifier that identifies the subagent.
	     *           Default is the null OID (no ID).
	     *
	     * \param unix_domain_socket The socket file to connect to.
             *                           Defaults to /var/agentx/master, as 
             *                           described in RFC 2741, section 8.2.1 
             *                           "Well-known Values".
	     */
	    master_proxy(std::string description="",
		   uint8_t default_timeout=0,
		   oid ID=oid(),
		   std::string unix_domain_socket="/var/agentx/master");

	    /**
	     * \brief Register a subtree with the master agent
	     *
	     * This function registers a subtree (or MIB region).
             * 
             * \internal
	     *
	     * This method adds the registered subtree to registrations on 
	     * success.
	     *
	     * \endinternal
	     *
	     * \param subtree The (root of the) subtree to register.
	     *
	     * \param priority The priority with which to register the subtree.
	     *                 Default is 127 according to RFC 2741, 6.2.3.  
	     *                 "The agentx-Register-PDU".
	     *
	     * \param timeout The timeout value for the registered subtree, in
	     *		      seconds. This value overrides the timeout of the 
	     *		      session.  Default value is 0 (no override) 
	     *		      according to RFC 2741, 6.2.3.  "The 
	     *		      agentx-Register-PDU".
	     *
	     * \exception disconnected If the master_proxy is currently in
	     *                         state 'disconnected'.
	     *
	     * \exception timeout_exception If the master agent does not
	     *                              respond within the timeout 
	     *                              interval.
	     *
	     * \exception master_is_unable The master agent was unable to
	     *                             perform the desired register 
	     *                             request.  The reason for that is 
	     *                             unknown.
	     *
	     * \exception duplicate_registration If the exact same subtree was
             *                                   already registered, either by 
             *                                   another subagent or by this 
             *                                   subagent.
	     *
	     * \exception master_is_unwilling If the master was unwilling for
	     *                                some reason to make the desired 
	     *                                registration.
	     *
	     * \exception parse_error A malformed network message was found
             *                        during communication with the master. 
             *                        This may be a programming error in the 
             *                        master or in the agentXcpp library. It is 
             *                        possible that the master actually 
             *                        performed the desired registration and 
             *                        that a retry will result in a 
             *                        duplicate_registration error.
	     */
	    void register_subtree(oid subtree,
				  uint8_t priority=127,
				  uint8_t timeout=0);

	    /**
	     * \brief Unregister a subtree with the master agent
	     *
	     * This function unregisters a subtree (or MIB region) which has 
	     * previously been registered.
             * 
             * \internal
	     *
	     * This method removes the registered subtree from
	     * registrations.
	     * 
	     * \endinternal
	     *
	     * \param subtree The (root of the) subtree to unregister.
	     *
	     * \param priority The priority with which the registration was
             *                 done.
	     *
	     * \exception disconnected If the master_proxy is currently in
	     *                         state 'disconnected'.
	     *
	     * \exception timeout_error If the master agent does not
	     *                          respond within the timeout interval.
	     *
	     * \exception master_is_unable The master agent was unable to
	     *                             perform the desired unregister 
	     *                             request.  The reason for that is 
	     *                             unknown.
	     *
	     * \exception unknown_registration The MIB region is not currently
	     *                                 registered with this parameters.
	     *
	     * \exception parse_error A malformed network message was found
             *                        during communication with the master. 
             *                        This may be a programming error in the 
             *                        master or in the agentXcpp library. It is 
             *                        possible that the master actually 
             *                        unregistered the MIB region.
	     */
            // TODO: the 'priority' parameter can possibly be omitted: the 
            // value can be stored by master_agent upon subtree registration.
	    void unregister_subtree(oid subtree,
				    uint8_t priority=127);

            /**
	     * \brief Check whether the session is in state connected
	     *
	     * \returns true if the session is connected, false otherwise.
	     */
	    bool is_connected()
	    {
		return this->connection->is_connected();
	    }

	    /**
	     * \brief Connect to the master agent.
	     *
             * \note Upon creation of a master_proxy object, the connection is
             *       automatically established. If the current state is 
             *       "connected", the function does nothing.
	     *
	     * \exception disconnected If connecting fails.
	     */
	    void connect();

	    /**
	     * \brief Shutdown the session.
	     *
	     * Disconnect from the master agent.
	     * 
             * \note Upon destruction of a master_proxy object the session is
             *       automatically shutdown. If the session is in state 
             *       "disconnected", this function does nothing.
	     *
	     * \param reason The shutdown reason is reported to the master
	     *               agent during shutdown.
	     *
	     * \exception None.
	     */
	    void disconnect(ClosePDU::reason_t reason=ClosePDU::reasonShutdown);

	    /**
	     * \brief Reconnect to the master agent.
	     *
	     * Disconnects from the master (only if currently connected), then 
	     * connects again.
	     *
	     * \exception disconnected If connecting fails.
	     */
	    void reconnect()
	    {
		this->connection->disconnect();
	
                // throws disconnected, which is forwarded:
		this->connection->connect();
	    }

	    /**
             * \brief Get the sessionID of the session.
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
		return this->sessionID;
	    }

	    /**
             * \brief Destructor.
	     *
             * The destructor cleanly shuts down the session (if it is 
             * currently established) with the reason 'Shutdown' and destroys 
             * the master_proxy object.
             */
	    ~master_proxy();

	    /**
             * \brief Add an SNMP variable for serving.
	     *
             * This adds an SNMP variable which can then be read and/or 
             * written.
	     *
	     * Variables can only be added to MIB regions which were registered 
	     * in advance.
	     *
             * If adding a variable with an id for which another variable is 
             * already registered, it replaces the old one.
             *
	     * \param id The OID of the variable.
	     *
	     * \param v The variable.
	     *
	     * \exception unknown_registration If trying to add a variable
	     *                                 with an id which does not reside 
	     *                                 within a registered MIB 
	     *                                 region.
	     */
	    void add_variable(const oid& id, shared_ptr<variable> v);

	    /**
	     * \brief Remove an SNMP variable so that is not longer accessible.
	     *
	     * This removes a variable previously added using add_variable().  
	     * The variable will no longer receive SNMP requests.
	     *
	     * If no variable is known for the given id, nothing happens.
	     *
	     * \param id The OID of the variable to remove. This is the OID
	     *           which was given to add_variable().
	     *
	     * \exception None.
	     */
	    void remove_variable(const oid& id);
    };
}

#endif
