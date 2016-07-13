#include "ws_fault.h"

/**
 * c++ ctor
 */
WSFault::WSFault
(){};

/**
 * c++ dtor
 */
WSFault::~WSFault
(){};

/**
 * PHP __ctor
 *
 * WSFault::__construct()
 */
void WSFault::__construct
(){};

/**
 *	Get Soap fault code
 */
Php::Value WSFault :: getCode()
{
	return _code.empty() ? "" : _code;
}

/**
 *	Get Soap fault reason, ( Human readable )
 */
Php::Value WSFault :: getReason()
{
	return _reason.empty() ? "" : _reason;
}

/**
 * Get Role Identifies the role in which the SOAP processing node was operating at the time the fault occurred.
 */
Php::Value WSFault :: getRole()
{
	return _role.empty() ? "" : _role;
}

/**
 * Get Details Contains application specific error information.
 */
Php::Value WSFault :: getDetails()
{
	return _details.empty() ? "" : _details;
}

/**
 * Get Node  Contains the received SOAP fault as an XML string.
 */
Php::Value WSFault :: getXMLNode()
{
	return _node.empty() ? "" : _node;
}
