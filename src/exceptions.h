#ifndef __EXCEPTIONS_H__
#define __EXCEPTIONS_H__

// Exception for parse errors
class parse_error { };

// Invalid parameter.
class inval_param { };

// Received PDU with wrong AgentX protocol version
class version_mismatch { };

#endif
