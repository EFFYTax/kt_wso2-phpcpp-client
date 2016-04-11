#ifndef WS_CLIENT_PROXY_H
#define WS_CLIENT_PROXY_H

#include <phpcpp.h>

class WSClientProxy : public Php::Base {

public:

	WSClientProxy();

	virtual ~WSClientProxy();

	void __construct();
};

#endif
