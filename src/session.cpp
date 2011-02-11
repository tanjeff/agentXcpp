#include "session.h"
#include "OpenPDU.h"
#include "ClosePDU.h"
#include "helper.h"
#include "types.h"

#include <iostream>
using namespace std;

using namespace agentx;

session::session(std::string filename)
: socket(io_service),
    endpoint(filename.c_str())
{
    cout << filename << endl;

    // Connect to endpoint
    socket.connect(endpoint);
    
    // Send OpenPDU
    OpenPDU openpdu;
    data_t buf = openpdu.serialize();
    socket.send(boost::asio::buffer(buf.c_str(), buf.size()));

    // Get response
    PDU* response = PDU::get_pdu(socket);

    // Get sessionID
    sessionID = response->get_sessionID();

    cout << "received sessionID " << sessionID << endl;
}

session::~session()
{
    cout << "session::~session(): sending ClosePDU" << endl;
    
    ClosePDU pdu(sessionID, ClosePDU::reasonShutdown);
    data_t buf = pdu.serialize();
    socket.send(boost::asio::buffer(buf.c_str(), buf.size()));
    socket.close();
}
