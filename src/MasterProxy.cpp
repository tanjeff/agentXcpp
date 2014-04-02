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
#include <QtGlobal>

#include "MasterProxy.hpp"
#include "OpenPDU.hpp"
#include "ClosePDU.hpp"
#include "ResponsePDU.hpp"
#include "RegisterPDU.hpp"
#include "GetPDU.hpp"
#include "GetNextPDU.hpp"
#include "NotifyPDU.hpp"
#include "util.hpp"


using namespace std;
using namespace agentxcpp;








MasterProxy::MasterProxy(std::string _description,
			   quint8 _default_timeout,
			   OidVariable _id,
			   std::string _filename) :
    socket_file(_filename.c_str()),
    sessionID(0),
    description(_description),
    default_timeout(_default_timeout),
    id(_id)
{
    // Initialize connector (never use timeout=0)
    quint8 timeout;
    timeout = (this->default_timeout == 0) ? 1 : this->default_timeout;
    connection = new UnixDomainConnector(
			       _filename.c_str(),
			       timeout*1000);
    connection->moveToThread(&m_thread);
    m_thread.start();


    // Register this object as %PDU handler
    //this->connection->register_handler( this );

    // Try to connect
    try
    {
	// throws disconnected:
	this->connect();
    }
    catch(disconnected)
    {
	// Ignore, stay disconnected
    }
    catch(...)
    {
	// Ignore, stay disconnected
    }

}


void MasterProxy::connect()
{
//    if( this->connection->is_connected() )
//    {
//	// we are already connected -> nothing to do
//	return;
//    }

    // Clear registrations and variables
    registrations.clear();
    variables.clear();

    // Connect to endpoint
    this->connection->connect();

    // The response we expect from the master
    QSharedPointer<ResponsePDU> response;

    try
    {
	// Send OpenPDU
	QSharedPointer<OpenPDU> openpdu(new OpenPDU);
	openpdu->set_timeout(default_timeout);
	openpdu->set_id(id);
	// throws disconnected and timeout_error:
	response = this->connection->request(openpdu);

//	// Wait for response
//	// throws disconnected and timeout_error:
//	response = this->connection->wait_for_response(openpdu.get_packetID());
    }
    catch(disconnected)
    {
	throw; // forward
    }
    catch(timeout_error)
    {
	throw disconnected();
    }

    // Check for errors
    if(response->get_error() != ResponsePDU::noAgentXError)
    {
//	// Some error occured, disconnect
//	this->connection->disconnect();
	throw disconnected();
    }

    // All went fine, we are connected now
    this->sessionID = response->get_sessionID();

    QObject::connect(connection,
                     SIGNAL(pduArrived(QSharedPointer<PDU>)),
                     this,
                     SLOT(handle_pdu(QSharedPointer<PDU>)));
}




void MasterProxy::disconnect(ClosePDU::reason_t reason)
{
//    if( ! this->connection->is_connected() )
//    {
//	// we are already disconnected -> nothing to do
//	return;
//    }

    // According to RFC 2741, 7.1.8. "Processing the agentx-Close-PDU", the 
    // master agent unregisters all MIB regions, frees all index values and all 
    // sysORID are removed. Thus no need to clean up before ClosePDU is sent.

    // The response we expect from the master
    QSharedPointer<ResponsePDU> response;

    // Try clean shutdown (ignore errors)
    try
    {
	// Unregister stuff if any
	std::list< QSharedPointer<RegisterPDU> >::const_iterator r;
	r = this->registrations.begin();
	while (r != this->registrations.end())
	{
	    this->undo_registration(create_unregister_pdu(*r));
	    r++;
	}

	// Send ClosePDU
	QSharedPointer<ClosePDU> closepdu(new ClosePDU(this->sessionID, reason));
	// throws disconnected and timeout_error:
	response = this->connection->request(closepdu);
	
//	// Wait for response
//	// throws disconnected and timeout_error:
//	response = this->connection->wait_for_response(closepdu.get_packetID());

	// Check for errors
	if(response->get_error() != ResponsePDU::noAgentXError)
	{
	    // Some error occured. However, We will soon disconnect anyway.
	    // -> ignore
	    // TODO: Insert log message here.
	}
    }
    catch(...)
    {
	// We will soon disconnect anyway.
	// -> ignore all errors
    }

    // Finally: disconnect
//    this->connection->disconnect();
}

MasterProxy::~MasterProxy()
{
    // Disconnect from master agent
    this->disconnect(ClosePDU::reasonShutdown);

    // Destroy connection
    // Unregistering this object as %PDU handler is unneeded.
    delete this->connection;
}


void MasterProxy::do_registration(QSharedPointer<RegisterPDU> pdu)
{
    // Are we connected?
//    if( ! is_connected())
//    {
//	throw(disconnected());
//    }

    // Send RegisterPDU
    // (forward exceptions timeout_error and disconnected)
    QSharedPointer<ResponsePDU> response;
    response = this->connection->request(pdu);

//    // Wait for response
//    // (forward exceptions timeout_error and disconnected)
//    response = this->connection->wait_for_response(pdu->get_packetID());

    // Check Response
    switch(response->get_error())
    {
	// General errors:

	case ResponsePDU::parseError:
	    // Oops, we sent a malformed PDU to the master
	    throw(internal_error());

	case ResponsePDU::notOpen:
	    // We checked the connection state before, but maybe we lost the 
	    // connection during communication...
	    throw(disconnected());

	case ResponsePDU::unsupportedContext:
	    // We do currently not really support contexts in this library. An 
	    // invalid context is thus probably an agentxcpp bug.
	    throw(internal_error());

	case ResponsePDU::processingError:
	    // master was unable to process the request
	    throw(master_is_unable());

	case ResponsePDU::noAgentXError:
	    // Hey, it worked!
	    break;

	// Register-specific errors:

	case ResponsePDU::duplicateRegistration:
	    throw(duplicate_registration());

	case ResponsePDU::requestDenied:
	    throw(master_is_unwilling());

	default:
	    // This is a case of can-not-happen. Probably the master is buggy.  
	    // The agentxcpp library is bug-free of course ;-)
	    // We throw a parse error meanwhile, because we didn't expect the 
	    // response to look like that...
	    throw(parse_error());
    }

    // Finish
    return;
}





void MasterProxy::register_subtree(OidVariable subtree,
		      quint8 priority,
		      quint8 timeout)
{
    // Build PDU
    QSharedPointer<RegisterPDU> pdu(new RegisterPDU);
    pdu->set_subtree(subtree);
    pdu->set_priority(priority);
    pdu->set_timeout(timeout);
    pdu->set_sessionID(this->sessionID);

    // Send PDU
    try
    {
	this->do_registration(pdu);
    }
    catch( internal_error )
    {
	// Huh, it seems that we sent a malformed PDU to the master. We convert 
	// this to parse_error.
	throw(parse_error());
    }
    catch(...)
    {
	// All other exceptions are forwarded unmodified:
	throw;
    }

    // Success: store registration
    this->registrations.push_back(pdu);

}



void MasterProxy::unregister_subtree(OidVariable subtree,
				      quint8 priority)
{
    // The UnregisterPDU
    QSharedPointer<UnregisterPDU> pdu;

    // Remove the registration from registrations list
    std::list< QSharedPointer<RegisterPDU> >::iterator r;
    r = this->registrations.begin();
    while (r != this->registrations.end())
    {
	if(   (*r)->get_priority() == priority
	   && (*r)->get_subtree() == subtree
	   && (*r)->get_range_subid() == 0
	   && (*r)->get_upper_bound() == 0 )
	{
	    // registration found

	    // create UnregisterPDU
	    pdu = create_unregister_pdu(*r);

	    // remove registration from list, forward to next one
	    r = registrations.erase(r);
	}
	else
	{
	    // Ignore registration, inspect next one
	    r++;
	}
    }

    // Sent PDU
    try
    {
	this->undo_registration(pdu);
    }
    catch( internal_error )
    {
	// Huh, it seems that we sent a malformed PDU to the master. We convert 
	// this to parse_error.
	throw(parse_error());
    }
    catch(...)
    {
	// All other exceptions are forwarded unmodified:
	throw;
    }
}



void MasterProxy::undo_registration(QSharedPointer<UnregisterPDU> pdu)
{
    // Are we connected?
//    if( ! is_connected())
//    {
//	throw(disconnected());
//    }

    // Send UnregisterPDU
    // (forward exceptions timeout_error and disconnected)
    QSharedPointer<ResponsePDU> response;
    response = this->connection->request(pdu);

//    // Wait for response
//    // (forward exceptions timeout_error and disconnected)
//    QSharedPointer<ResponsePDU> response;
//    response = this->connection->wait_for_response(pdu->get_packetID());

    // Check Response
    switch(response->get_error())
    {
	// General errors:

	case ResponsePDU::parseError:
	    // Oops, we sent a malformed PDU to the master
	    throw(internal_error());

	case ResponsePDU::notOpen:
	    // We checked the connection state before, but maybe we lost the 
	    // connection during communication...
	    throw(disconnected());

	case ResponsePDU::unsupportedContext:
	    // We do currently not really support contexts in this library. An 
	    // invalid context is thus probably an agentxcpp bug.
	    throw(internal_error());

	case ResponsePDU::processingError:
	    // master was unable to process the request
	    throw(master_is_unable());

	case ResponsePDU::noAgentXError:
	    // Hey, it worked!
	    break;

	// Register-specific errors:

	case ResponsePDU::unknownRegistration:
	    throw(unknown_registration());

	default:
            // This is a case of can-not-happen. Probably the master is buggy. 
            // The agentxcpp library is bug-free of course ;-)
	    // We throw a parse error meanwhile, because we didn't expect the 
	    // response to look like that...
	    throw(parse_error());
    }

    // Finish
    return;
}



QSharedPointer<UnregisterPDU> MasterProxy::create_unregister_pdu(
				    QSharedPointer<RegisterPDU> pdu)
{
    QSharedPointer<UnregisterPDU> new_pdu(new UnregisterPDU());
    new_pdu->set_subtree( pdu->get_subtree() );
    new_pdu->set_range_subid( pdu->get_range_subid() );
    new_pdu->set_upper_bound( pdu->get_upper_bound() );
    new_pdu->set_priority( pdu->get_priority() );

    return new_pdu;
}


void MasterProxy::handle_getpdu(QSharedPointer<ResponsePDU> response, QSharedPointer<GetPDU> get_pdu)
{
        // Handling according to
	// RFC 2741, 7.2.3.1 "Subagent Processing of the agentx-Get-PDU"

	// Extract searchRange list
	vector<OidVariable> sr = get_pdu->get_sr();

	// Iterate over list and handle each OidVariable separately
	vector<OidVariable>::const_iterator i;
        quint16 index = 1;  // Index is 1-based (RFC 2741,
                             // 5.4. "Value Representation"):
	for(i = sr.begin(); i != sr.end(); i++)
	{
	    // The name
	    const OidVariable& name = *i;

	    // Find variable for current OID
	    map< OidVariable, QSharedPointer<AbstractVariable> >::const_iterator var;
	    var = variables.find(name);
	    if(var != variables.end())
	    {
		// Step (2): We have a variable for this OidVariable

		// update variable
                try
                {
                    // Add variable to response (Step (1): include name)
                    var->second->handle_get();
                    response->varbindlist.push_back( varbind(name, var->second) );
                }
                catch(...)
                {
                    // An error occurred
                    response->set_error( ResponsePDU::genErr );
                    response->set_index( index );
                    // Leave response.varbindlist empty
                }

	    }
	    else
	    {
		// Interpret 'name' as prefix:
		// append .0 and check whether we have a variable
		// with this name
		OidVariable name_copy(name, 0);

		var = variables.find(name_copy);
		if(var != variables.end())
		{
		    // Step (4): We have a variable with the object
		    //           identifier prefix 'name': Send noSuchInstance 
		    //           error (Step (1): include name)
		    response->varbindlist.push_back( varbind(name, varbind::noSuchInstance) );
		}
		else
		{
		    // Step (3): we have no variable with the object
		    //           identifier prefix 'name': Send noSuchObject 
		    //           error (Step (1): include name)
		    response->varbindlist.push_back( varbind(name, varbind::noSuchObject) );
		}
	    }

            index++;
	}
}



void MasterProxy::handle_getnextpdu(QSharedPointer<ResponsePDU> response, QSharedPointer<GetNextPDU> getnext_pdu)
{
        // Handling according to
	// RFC 2741, 7.2.3.2 "Subagent Processing of the agentx-GetNext-PDU"

	// Extract searchRange list
	vector< pair<OidVariable,OidVariable> >& sr = getnext_pdu->get_sr();

	// Iterate over list and handle each SearchRange separately
	vector< pair<OidVariable,OidVariable> >::const_iterator i;
        quint16 index = 1;  // Index is 1-based (RFC 2741,
                             // 5.4. "Value Representation"):
	for(i = sr.begin(); i != sr.end(); i++)
	{
	    // The names
	    const OidVariable& starting_oid = i->first;
            const OidVariable& ending_oid   = i->second;

            // Find "next" variable
	    map< OidVariable, QSharedPointer<AbstractVariable> >::const_iterator next_var;
	    if( ! starting_oid.get_include())
            {
                // Find the closest lexicographical successor to the starting 
                // OID (excluding the starting OID itself)
                next_var = variables.upper_bound(starting_oid);
            }
            else
            {
                // Find the exact variable or, if not present, find the 
                // lexicographical successor of it
                next_var = variables.lower_bound(starting_oid);
            }
            if( ! ending_oid.is_null() )
            {
                // The "next" variable must precede the ending OID (it must not 
                // be greater or equal than the ending OID)
                if( next_var->first >= ending_oid )
                {
                    // The found "next" variable doesn't precede the ending 
                    // OID, which means that we didn't found a suitable 
                    // variable.
                    next_var = variables.end(); // indicate "not found"
                }
            }

	    if(next_var != variables.end())
	    {
                // "Next" variable was found

		// update variable
                try
                {
                    next_var->second->handle_get();
                    response->varbindlist.push_back( varbind(next_var->first, next_var->second) );
                }
                catch(...)
                {
                    // An error occurred
                    response->set_error( ResponsePDU::genErr );
                    response->set_index( index );
                    // Leave response.varbindlist empty
                }

	    }
	    else
	    {
                // "Next" variable was NOT found
		response->varbindlist.push_back( varbind(starting_oid, varbind::endOfMibView) );
	    }

            index++;
	}
}



void MasterProxy::handle_testsetpdu(QSharedPointer<ResponsePDU> response, QSharedPointer<TestSetPDU> testset_pdu)
{
    // Handling according to
    // RFC 2741, 7.2.4.1 "Subagent Processing of the agentx-TestSet-PDU"

    // Extract Varbind list
    vector<varbind>& vb = testset_pdu->get_vb();

    // Initially, no Varbind failed:
    response->set_error(ResponsePDU::noAgentXError);

    // Iterate over list and handle each Varbind separately. Return on the 
    // first varbind which doesn't validate correctly.
    vector<varbind>::const_iterator i;
    quint16 index;
    for(i = vb.begin(), index = 1; i != vb.end(); i++, index++)
    {
        // Find the associated variable
        map< OidVariable, QSharedPointer<AbstractVariable> >::const_iterator var;
	var = variables.find(i->get_name());
        if(var == variables.end())
        {
            // error: variable unknown
            response->set_error(ResponsePDU::notWritable);
            response->set_index(index);

            // Some variables may have allocated resources, which must be
            // released again. This is the same as handle_cleanupsetpdu() does, 
            // so we are lazy here and call this function:
            this->handle_cleanupsetpdu();
            return;
        }

        // Remember the found variable for later operations
        setlist.push_back(var->second);

        // Perform validation, store result within response
        // Note: ResponsePDU::error_t and variable::testset_result_t are in 
        // sync, therefore the static cast works.
        response->set_error(static_cast<ResponsePDU::error_t>(var->second->handle_testset(i->get_var())));
        if(response->get_error() != ResponsePDU::noAgentXError)
        {
            response->set_index(index);

            // Some variables may have allocated resources, which must be
            // released again. This is the same as handle_cleanupsetpdu() does, 
            // so we are lazy here and call this function:
            this->handle_cleanupsetpdu();

            return;
        }
    }

    // At this point, all VarBinds validated correctly. The response was filled 
    // correctly, and we simply return.
    return;
}


void MasterProxy::handle_cleanupsetpdu()
{
    // Handling according to
    // RFC 2741, 7.2.4.4 "Subagent Processing of the agentx-CleanupSet-PDU"

    // Iterate over list and handle each Varbind separately. We iterate 
    // backwards, so that resources are released in the reverse order of their
    // allocation.
    list< QSharedPointer<AbstractVariable> >::const_reverse_iterator i;
    for(i = setlist.rbegin(); i != setlist.rend(); i++)
    {
        (*i)->handle_cleanupset();
    }

    // Finally: remove all variables from the list
    setlist.clear();
}

void MasterProxy::handle_commitsetpdu(QSharedPointer<ResponsePDU> response, QSharedPointer<CommitSetPDU> commitset_pdu)
{
    // Handling according to RFC 2741, 7.2.4.2 "Subagent Processing of the 
    // agentx-CommitSet-PDU"

    // Iterate over list and handle each Varbind separately.
    list< QSharedPointer<AbstractVariable> >::iterator i;
    quint16 index = 1;  // Index is 1-based (RFC 2741, 5.4. "Value Representation")
    for(i = setlist.begin(); i != setlist.end(); i++)
    {
        if( (*i)->handle_commitset() )
        {
            // operation succeeded
            response->set_error(ResponsePDU::noAgentXError);
            response->set_index(0);
        }
        else
        {
            // operation failed: store index of failed varbind and do not process further variables.
            response->set_error(ResponsePDU::commitFailed);
            response->set_index(index);
            return;
        }
        index++;
    }
}


void MasterProxy::handle_undosetpdu(QSharedPointer<ResponsePDU> response, QSharedPointer<UndoSetPDU> undoset_pdu)
{
    // Handling according to RFC 2741, 7.2.4.3 "Subagent Processing of the
    // agentx-UndoSet-PDU"

    bool failed = false;

    // Iterate over list and handle each Varbind separately.
    list< QSharedPointer<AbstractVariable> >::iterator i;
    quint16 index = 1;  // Index is 1-based (RFC 2741, 5.4. "Value Representation")
    for(i = setlist.begin(); i != setlist.end(); i++)
    {
        if( (*i)->handle_undoset() )
        {
            // operation succeeded
            response->set_error(ResponsePDU::noAgentXError);
            response->set_index(0);
        }
        else
        {
            if(failed == false)
            {
                // operation failed: store index of failed varbind and do not process further variables.
                response->set_error(ResponsePDU::undoFailed);
                response->set_index(index);

                // Note: it is unclear whether processing should stop immediately
                //       after the first failed UndoSet. We risk that some variables
                //       get neither an UndoSet nor a CleanupSet call. Therefore,
                //       we continue processing and report the first failed
                //       variable for now.
                failed = true;
            }
        }
        index++;
    }
}


void MasterProxy::handle_pdu(QSharedPointer<PDU> pdu)
{
    int error = 0; // 0 is "success"
    if(error == -2)
    {
	// Version error
	// TODO: provide better handling. For now: ignore PDU
        // Note: Error numbers are documented in connector.hpp
	return;
    }

    // Here we process all PDU's except ResponsePDU's, according to RFC 2741, 
    // 7.2.2. "Subagent Processing".

    // Step 2) Was there a parse error?
    //
    // Note: Error numbers are documented in connector.hpp
    if(error == -1)
    {
        // Step 4b) Stop processing, don't send reply.
	//
	// Note: we cannot determine whether the header was sucessfully
	//       parsed, therefore we simply ignore the PDU. We don't send a 
	//       parseError indication to the master agent.
	return;
    }

    // Step 1) Create a response and copy header from received pdu
    //
    // Notes:
    //   - Only if no parse error was detected
    //   - The version, type and payload_length fields are filled
    //     automatically
    //   - The flags are not copied, because they have
    //     other meanings in ResponsePDU's.
    //   - TODO: Context is not yet supported.
    QSharedPointer<ResponsePDU> response(new ResponsePDU);
    response->set_sessionID( pdu->get_sessionID() );
    response->set_transactionID( pdu->get_transactionID() );
    response->set_packetID( pdu->get_packetID() );
    response->set_error(ResponsePDU::noAgentXError);
    response->set_index(0);

    // Step 3) Is the session valid?
    if(pdu->get_sessionID() != this->sessionID)
    {
	response->set_error(ResponsePDU::notOpen);

	// Step 4a) Stop processing the PDU. Send response.
	try
	{
//	    this->connection->send(response);
	    connection->send(response);
	    //QMetaObject::invokeMethod(connection, "do_send", Q_ARG(QSharedPointer<PDU>, response));
	}
	catch(timeout_error) { /* connection loss. Ignore.*/ }
	catch(disconnected) { /* connection loss. Ignore.*/ }

	return;
    }

    //
    // Next thing to do: determine PDU type and handle it.
    //

    // Is it a GetPDU?
    QSharedPointer<GetPDU> get_pdu;
    if( (get_pdu = qSharedPointerDynamicCast<GetPDU>(pdu)) != 0 )
    {
        // (response is modified in-place)
        this->handle_getpdu(response, get_pdu);
    }

    // Is it a GetNextPDU?
    QSharedPointer<GetNextPDU> getnext_pdu;
    if( (getnext_pdu = qSharedPointerDynamicCast<GetNextPDU>(pdu)) != 0 )
    {
        // (response is modified in-place)
        this->handle_getnextpdu(response, getnext_pdu);
    }

    // Is it a TestSetPDU?
    QSharedPointer<TestSetPDU> testset_pdu;
    if( (testset_pdu = qSharedPointerDynamicCast<TestSetPDU>(pdu)) != 0 )
    {
        // (response is modified in-place)
        this->handle_testsetpdu(response, testset_pdu);
    }

    // Is it a CleanupSetPDU?
    QSharedPointer<CleanupSetPDU> cleanupset_pdu;
    if( (cleanupset_pdu = qSharedPointerDynamicCast<CleanupSetPDU>(pdu)) != 0 )
    {
        this->handle_cleanupsetpdu();

        // Do not send a response:
        return;
    }

    // Is it a CommitSetPDU?
    QSharedPointer<CommitSetPDU> commitset_pdu;
    if( (commitset_pdu = qSharedPointerDynamicCast<CommitSetPDU>(pdu)) != 0 )
    {
        // (response is modified in-place)
        this->handle_commitsetpdu(response, commitset_pdu);
    }

    // Is it an UndoSetPDU?
    QSharedPointer<UndoSetPDU> undoset_pdu;
    if( (undoset_pdu = qSharedPointerDynamicCast<UndoSetPDU>(pdu)) != 0 )
    {
        // (response is modified in-place)
        this->handle_undosetpdu(response, undoset_pdu);
    }

    // TODO: handle other PDU types

    // Finally: send the response
    try
    {
        connection->send(response);
        //QMetaObject::invokeMethod(connection, "do_send", Q_ARG(QSharedPointer<PDU>, response));
        //this->connection->send(response);
    }
    catch(timeout_error) { /* connection loss. Ignore.*/ }
    catch(disconnected) { /* connection loss. Ignore.*/ }
}

void MasterProxy::addVariables(QVector< QPair<
                            OidVariable, QSharedPointer<AbstractVariable> > > v)
{
    QVectorIterator<QPair< OidVariable,
                           QSharedPointer<AbstractVariable> > > iter(v);
    while(iter.hasNext())
    {
        QPair<OidVariable, QSharedPointer<AbstractVariable> > varPair;
        varPair = iter.next();

        add_variable(varPair.first, varPair.second);
    }
}

void MasterProxy::add_variable(const OidVariable& id, QSharedPointer<AbstractVariable> v)
{
    // Check whether id is contained in a registration
    bool is_registered = false;
    std::list< QSharedPointer<RegisterPDU> >::const_iterator r;
    for(r = registrations.begin(); r != registrations.end(); r++)
    {
	if((*r)->get_instance_registration() == false &&
	   (*r)->get_range_subid() == 0)
	{
	    // Registration is a simple subtree
	    if( (*r)->get_subtree().contains(id) )
	    {
		// The ID lies within a registered area
		is_registered = true;
		break; // stop search
	    }
	}
	// TODO: handle other registrations (e.g. instance registration)
    }

    if( ! is_registered )
    {
	// Not in a registered area
	throw(unknown_registration());
    }
    variables[id] = v;
}


bool MasterProxy::isRegistered(OidVariable id)
{
    // Check whether id is contained in a registration
    bool is_registered = false;
    std::list< QSharedPointer<RegisterPDU> >::const_iterator r;
    for(r = registrations.begin(); r != registrations.end(); r++)
    {
        if((*r)->get_instance_registration() == false &&
           (*r)->get_range_subid() == 0)
        {
            // Registration is a simple subtree
            if( (*r)->get_subtree().contains(id) )
            {
                // The ID lies within a registered area
                is_registered = true;
                break; // stop search
            }
        }
        // TODO: handle other registrations (e.g. instance registration)
    }
    return is_registered;
}



void MasterProxy::remove_variable(const OidVariable& id)
{
    // Remove variable
    variables.erase(id); // If variable was not registered: ignore
}

void MasterProxy::removeVariables(const QVector<OidVariable>& ids)
{
    QVectorIterator<OidVariable> iter(ids);
    while(iter.hasNext())
    {
        remove_variable(iter.next());
    }
}

void MasterProxy::send_notification(const OidVariable& snmpTrapOID,
                                    const TimeTicksVariable* sysUpTime,
                                    const vector<varbind>& varbinds)
{
    QSharedPointer<NotifyPDU> pdu(new NotifyPDU);
    pdu->set_sessionID(this->sessionID);

    vector<varbind>& vb = pdu->get_vb();

    // First of all: add mandatory sysUpTime (if given)
    if(sysUpTime)
    {
        QSharedPointer<TimeTicksVariable> value(new TimeTicksVariable(*sysUpTime));
        vb.push_back(varbind(OidVariable(sysUpTime_oid, "0"), value));
    }

    // Second: add mandatory snmpTrapOID
    QSharedPointer<OidVariable> trapoid(new OidVariable(snmpTrapOID));
    vb.push_back(varbind(OidVariable(snmpTrapOID_oid, "0"), trapoid));

    // Append given varbinds
    vb.insert(vb.end(), varbinds.begin(), varbinds.end());

    // Send notification
    // Note: timeout_error and disconnected exceptions are forwarded.
    QSharedPointer<ResponsePDU> response;
    response = connection->request(pdu);

//    // Wait for response
//    // Note: timeout_error and disconnected exceptions are forwarded.
//    QSharedPointer<ResponsePDU> response;
//    response = connection->wait_for_response(pdu.get_packetID());

    // Handle response
    switch(response->get_error())
    {
        case ResponsePDU::processingError:
            // Can occur when snmpTrapOID.0 was missing in notification (but
            // we know that we don't make mistakes ^^) or when master could
            // not allocate resources.
            throw(master_is_unable());
            break;

        case ResponsePDU::unsupportedContext:
            // context not supported by master
            throw(unsupported_context());
            break;

        case ResponsePDU::noAgentXError:
            // All went well
            break;

        case ResponsePDU::parseError:
            // We sent garbage
        case ResponsePDU::notOpen:
            // Session was not open
        default:
            // We don't expect other errors as the ones handled above
            // Close connection
            disconnect(ClosePDU::reasonProtocolError);
            throw(disconnected());
            break;
    }
}
