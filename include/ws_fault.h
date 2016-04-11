#ifndef WS_FAULT_H
#define WS_FAULT_H

#include <phpcpp.h>

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
};

#endif
