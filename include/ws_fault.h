#ifndef WS_FAULT_H
#define WS_FAULT_H

#include <phpcpp.h>

using namespace std;

class WSFault :public Php::Base {

public:
	/**
	 * c++ ctor
	 */
	WSFault();

	/**
	 * c++ dtor
	 */
	virtual ~WSFault();

	/**
	 * PHP __ctor
	 *
	 * WSFault::__construct()
	 */
	void __construct();

	string _code;
	string _reason;
	string _role;
	string _details;
	string _node;

	/**
	 * Get SOAP fault code. SOAP fault codes are intended to provide a means by which faults are classified.
	 */
	Php::Value getCode();

	/**
	 * SOAP fault reason. Provides a human readable explanation of the fault.
	 */
	Php::Value getReason();

	/**
	 * SOAP fault role. Identifies the role in which the SOAP processing node was operating at the time the fault occurred.
	 */
	Php::Value getRole();

	/**
	 * SOAP fault detail. Contains application specific error information.
	 */
	Php::Value getDetails();

	/**
	 * Contains the received SOAP fault as an XML string.
	 */
	Php::Value getXMLNode();
};

#endif
