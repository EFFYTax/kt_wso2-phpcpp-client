#include "ws_client.h"

/**
 * WSClient c++ ctor
 */
WSClient::WSClient() : Axis2Client
(){};

/**
 * WSClient c++ dtor
 */
WSClient :: ~WSClient
(){};

/**
 * WSClient PHP __ctor
 *
 * {{{ proto void WSClient::__construct(string uri[, array options])
 */
void WSClient :: __construct() {

	//Check for axis2c service client ready
	if(!_svc_client) {
		throw Php::Exception("Failed to create axis2c service client - Check your configuration and log file");
	}
};

/**
 * Check if the WSClient has a WSMessage instance
 *
 * @access protected
 * @return bool
 */
bool WSClient :: hasMessage()
{
	return (_wsmessage) ? true : false;
};

/**
 * Check if the WSClient has a proxy configuration registered
 *
 * @access protected
 * @return bool
 */
bool WSClient :: hasProxy()
{
	return ( !_proxy_host.empty() && !_proxy_port.empty() ) ? true : false;
};

/**
 * Check if the WSClient has a proxy authentication registered, A proxy host/port configuration
 * must exist
 *
 * @access protected
 * @return bool
 */
bool WSClient :: hasProxyAuth()
{
	return (hasProxy() && !_proxy_username.empty() && !_proxy_password.empty()) ? true : false;
}

/**
 * Check if the WSClient has a SSL configuration. This need Axis2 must be compiled w/
 * openssl.
 *
 * @access protected
 * @return bool
 */
bool WSClient :: hasSSL()
{
	return (!_ssl_client_key_filename.empty() || !_ssl_server_key_filename.empty()) ? true : false;
}

/**
 * Check if the WSClient has a HTTP Authentication registered
 *
 * @access protected
 * @return bool
 */
bool WSClient :: hasHTTPAuth()
{
	return (!_http_username.empty() && !_http_password.empty()) ? true : false;
}

/**
 * Check if the WSClient has a WSSecurityToken configuration registered
 *
 * @access protected
 * @return bool
 */
bool WSClient :: hasSecurity()
{
	return (_wssectoken) ? true : false;
}

/*
 *  Sends a request payload in XML format and receives a response payload in the XML format.
 *
 *  The message payload is expected to be in valid XML format as expected by the service.
 *  If MTOM/XOP is to be used, you must use WSMessage to represent the message payload.
 *
 *  WSMessage WSClient::request()
 */
void WSClient :: request() {

	if(!_wsmessage) {
		throw Php::Exception("Need a WSMessage Instance");
	}

	int status_code = 0;

	log((boost::format("ktws::payload is ready :: %1%")
		% _wsmessage->getPayload()->get<string>()).str());

	_request_payload = get_node_by_xmlstr(_wsmessage->getPayload()->get<string>());

	if (!_request_payload)
	{
		log("request payload node is null");

		if(!_use_soap)
			throw Php::Exception( "request payload node is null" );
	}

	if(hasProxy()) 		register_proxy();
	if(hasProxyAuth())  register_proxy_auth();
	if(hasSSL())   		register_ssl();
	if(hasHTTPAuth())	register_http_auth();
	if(hasSecurity())   register_security();

	register_soap_version();
	register_http_transport();
	register_endpoint_and_soap_action();
	register_wsa_options();
	register_header();
	register_timeout();
	register_reliable_messaging();

	_response_payload = axis2_svc_client_send_receive (_svc_client, _env, _request_payload);

	if(_is_engaged_rm)
	{
		//TODO: implmement sequence and rm_spec_version
	}

	status_code = axis2_svc_client_get_http_status_code(_svc_client, _env);

	//REST
	if((status_code != 200) && (status_code != 202) && !_use_soap)
	{
		throw Php::Exception("Fault");
	}

	log("http status code :: " + std::to_string(status_code));

	if(has_soap_fault())
	{
		string fault = get_soap_fault_msg() ;

		throw Php::Exception("Fault :: " + fault );

	}
	else if(_response_payload)
	{
		_wsmessage->_response = get_response_msg();

		return ;
	}
};

/**
 * Disable soap protocol meaning using rest
 *
 * @access public
 * @return Php::Value<WSClient> instance
 */
Php::Value WSClient :: disable_soap()
{
	Php::Value self(this);

	_use_soap = false;

	self["use_soap"] = false;

	return this;
}

/**
 * Set the soap version and mark use_soap to be true
 *
 * @access public
 * @params Php::Parameters params
 * @return Php::Value<WSClient> instance
 */
Php::Value WSClient :: set_soap_version(Php::Parameters &params) {

	Php::Value self(this);
	Php::Value p = params[0];

	if(p.isScalar() && (p == 1.1 || p == 1.2))
		_soap_version = p.floatValue();

	else
		throw Php::Exception("Unexpected parameter for USE SOAP must be version or boolean");

	self[WSF_USE_SOAP] = p;

	return self;
};

/**
 * Set the http method to use
 *
 * @access public
 * @params Php::Parameters params
 * @return Php::Value<WSClient> instance
 */
Php::Value WSClient :: set_http_method(Php::Parameters &params) {

	Php::Value self(this);
	Php::Value p = params[0];

	string v = p;

	if(p.isString() && (
		   v == "POST"   || v == "post"
		|| v == "GET"    || v == "get"
		|| v == "PUT"    || v == "put"
		|| v == "DELETE" || v == "delete"
		))
	{
		_http_method = v;
	}
	else
		throw Php::Exception("Unexpected parameter for HTTP Method - Expected GET/POST/PUT/DELETE");

	self[WSF_HTTP_METHOD] = v;

	return self;
};

/**
 * Set transport URL ( NOT YET )
 *
 * @access public
 * @params Php::Parameters params
 * @return Php::Value<WSClient> instance
 */
Php::Value WSClient :: set_transport_url(Php::Parameters &params) {

	auto p = [params] ()->Php::Value {
		Php::Value p = params[0];
		return p.isArray() ? p[WSF_TRANSPORT_URL] : p;
	};
	Php::Value self(this);

	if(!p().isString())
		throw Php::Exception("Unexpected parameter for WSF Transport URL - Expected string");

	this->_transport_url = p().stringValue();
	self[WSF_TRANSPORT_URL] = p();

	return self;
};

/**
 * WSA Usage OK
 *
 * @access public
 * @params Php::Parameters params
 * @return Php::Value<WSClient> instance
 *
 */
Php::Value WSClient :: set_use_wsa(Php::Parameters &params) {

	Php::Value self(this);
	Php::Value p = params[0];

	if(p != "1.0" && p != "submission" && p != "disabled")
		throw Php::Exception("Unexpected parameter for USE WSA - read api doc" );

	_use_wsa = p.stringValue();

	self[WSF_USE_WSA] = _use_wsa;

	return this;
};

/**
 * Set WSA Address
 *
 * @access public
 * @params Php::Parameters params
 * @return Php::Value<WSClient> instance
 */
Php::Value WSClient :: set_wsa_address(Php::Parameters &params) {

	auto p = [params] ()->Php::Value {
		Php::Value p = params[0];
		return p.isArray() ? p[WSF_WSA_ADDRESS] : p;
	};
	Php::Value self(this);

	if(!p().isString() && !p().isArray()) {
		throw Php::Exception("Unexpected parameter for WSA Address - Expected string or array");
	}
	self[WSF_WSA_ADDRESS] = p();

	return self;
};

/**
 * set wsa reference params
 *
 * @access public
 * @params Php::Parameters params
 * @return Php::Value<WSClient> instance
 */
Php::Value WSClient :: set_wsa_ref_params(Php::Parameters &params) {

	auto p = [params] ()->Php::Value {
		Php::Value p = params[0];
		return p.isArray() ? p[WSF_WSA_REFERENCE_PARAMETERS] : p;
	};
	Php::Value self(this);

	if(!p().isArray())
	{
		throw Php::Exception("Unexpected parameter for WSA Ref Params - Expected array");
	}
	self[WSF_WSA_REFERENCE_PARAMETERS] = p();

	return self;
};

/**
 * set ssl server cert (OK)
 *
 * @access public
 * @params Php::Parameters params
 * @return Php::Value<WSClient> instance
 */
Php::Value WSClient :: set_server_cert(Php::Parameters &params) {

	Php::Value self(this);
	Php::Value p = params[0];

	if(p.isString() && !isFileExist(p))
	{
		throw Php::Exception("The server certificate supplied was not found on system" );
	}
	else if(!p.isString())
	{
		throw Php::Exception("Unexpected parameter for SSL Server Cert - Expected string" );
	}

	_ssl_server_key_filename = p.stringValue();

	self[WSF_SERVER_CERT] = p;

	return this;
};

/**
 * set ssl client cert (OK)
 *
 * @access public
 * @params Php::Parameters params
 * @return Php::Value<WSClient> instance
 */
Php::Value WSClient :: set_client_cert(Php::Parameters &params) {

	Php::Value self(this);
	Php::Value p = params[0];

	if(p.isString() && !isFileExist(p))
	{
		throw Php::Exception("The client certificate supplied was not found on system" );
	}
	else if(!p.isString())
	{
		throw Php::Exception("Unexpected parameter for SSL Client Cert - Expected string" );
	}

	_ssl_client_key_filename = p.stringValue();

	self[WSF_CLIENT_CERT] = p;

	return this;
};

/**
 * Set the SSL Passphrase (OK)
 *
 * @access public
 * @params Php::Parameters params
 * @return Php::Value<WSClient> instance
 */
Php::Value WSClient :: set_ssl_passphrase(Php::Parameters &params) {

	Php::Value self(this);
	Php::Value p = params[0];

	if(!p.isString())
	{
		throw Php::Exception("Unexpected parameter for SSL Passphrase - Expected string" );
	}

	_ssl_passphrase = p.stringValue();

	self[WSF_PASSPHRASE] = p;

	return self;
};

/**
 * set proxy host (OK)
 *
 * @access public
 * @params Php::Parameters params
 * @return Php::Value<WSClient> instance
 */
Php::Value WSClient :: set_proxy_host(Php::Parameters &params) {

	Php::Value p = params[0];
	Php::Value self(this);

	if(!p.isString()) {
		throw Php::Exception("Unexpected parameter for PROXY Host - Expected string" );
	}
	_proxy_host = p.stringValue();//
	self[WSF_PROXY_HOST] = p;

	return self;
};

/**
 *
 * @access public ( OK )
 *
 * @params Php::Parameters params
 * @return Php::Value<WSClient> instance
 */
Php::Value WSClient :: set_proxy_port(Php::Parameters &params) {

	Php::Value p = params[0];
	Php::Value self(this);

	if(!p.isScalar()) {
		throw Php::Exception("Unexpected parameter for PROXY Port - Expected scalar" );
	}
	_proxy_port = p.stringValue();
	self[WSF_PROXY_PORT] = p;

	return self;
};

/**
 * set XOP Response
 *
 * @access public
 * @params Php::Parameters params
 * @return Php::Value<WSClient> instance
 */
Php::Value WSClient :: set_xop_response(Php::Parameters &params) {

	auto p = [params] ()->Php::Value {
		Php::Value p = params[0];
		return p.isArray() ? p[WSF_RESPONSE_XOP] : p;
	};
	Php::Value self(this);

	if(!p().isBool()) {
		throw Php::Exception("Unexpected parameter for XOP Response - Expected boolean" );
	}
	self[WSF_RESPONSE_XOP] = p();

	return self;
};

/**
 *
 * @access public
 * @params Php::Parameters params
 * @return Php::Value<WSClient> instance
 */
Php::Value WSClient :: set_use_mtom(Php::Parameters &params) {

	auto p = [params] ()->Php::Value {
		Php::Value p = params[0];
		return p.isArray() ? p[WSF_USE_MTOM] : p;
	};
	Php::Value self(this);

	if(!p().isBool()) {
		throw Php::Exception("Unexpected parameter for USE MTOM - Expected scalar" );
	}
	self[WSF_USE_MTOM] = p();

	return self;
};

/**
 * set default attachement content type
 *
 * @access public
 * @params Php::Parameters params
 * @return Php::Value<WSClient> instance
 */
Php::Value WSClient :: set_default_attachement_content_type(Php::Parameters &params) {

	auto p = [params] ()->Php::Value {
		Php::Value p = params[0];
		return p.isArray() ? p[WSF_DEFAULT_ATTACHEMENT_CONTENT_TYPE] : p;
	};
	Php::Value self(this);

	if(!p().isBool() || !p().isString()) {
		throw Php::Exception("Unexpected parameter for Default attachement content type - Expected scalar" );
	}
	self[WSF_DEFAULT_ATTACHEMENT_CONTENT_TYPE] = p();

	return self;
};

/**
 * set policy name
 *
 * @access public
 * @params Php::Parameters params
 * @return Php::Value<WSClient> instance
 */
Php::Value WSClient :: set_policy_name(Php::Parameters &params) {

	auto p = [params] ()->Php::Value {
		Php::Value p = params[0];
		return p.isArray() ? p[WSF_POLICY_NAME] : p;
	};
	Php::Value self(this);

	if((!p().isObject() && !p().instanceOf("WSPolicy")) || !p().isArray()) {
		throw Php::Exception("Unexpected parameter for POLICY NAME - Expected Instance of WSPolicy or array" );
	}
	self[WSF_POLICY_NAME] = p();

	return self;
};

/**
 * set wsf reliable (OK)
 *
 * @access public
 * @params Php::Parameters params
 * @return Php::Value<WSClient> instance
 */
Php::Value WSClient :: set_ws_reliable(Php::Parameters &params) {

	Php::Value self(this);
	Php::Value p = params[0];

	if(!p.isScalar()) {
		throw Php::Exception("Unexpected parameter for WS Reliable- Expected scalar" );
	}

	if(p == 1.0)
	{
		_reliable_version = 1;
	}
	else if(p == 1.1)
	{
		_reliable_version = 2;
	}

	self[WSF_RELIABLE] = p;

	return self;
};

/**
 *
 * @access public
 * @params Php::Parameters params
 * @return Php::Value<WSClient> instance
 */
Php::Value WSClient :: set_sequence_expiry_time(Php::Parameters &params) {

	auto p = [params] ()->Php::Value {
		Php::Value p = params[0];
		return p.isArray() ? p[WSF_SEQUENCE_EXPIRY_TIME] : p;
	};
	Php::Value self(this);

	if(!p().isScalar()) {
		throw Php::Exception("Unexpected parameter for SEQ Expiry time - Expected scalar" );
	}
	self[WSF_SEQUENCE_EXPIRY_TIME] = p();

	return self;
};

/**
 * set continue sequence
 *
 * @access public
 * @params Php::Parameters params
 * @return Php::Value<WSClient> instance
 */
Php::Value WSClient :: set_contine_sequence(Php::Parameters &params) {

	auto p = [params] ()->Php::Value {
		Php::Value p = params[0];
		return p.isArray() ? p[WSF_WILL_CONTINUE_SEQUENCE] : p;
	};
	Php::Value self(this);

	if(!p().isScalar()) {
		throw Php::Exception("Unexpected parameter for CONTINUE SEQUENCE - Expected scalar" );
	}
	self[WSF_WILL_CONTINUE_SEQUENCE] = p();

	return self;
};

/**
 * set sequence key
 *
 * @access public
 * @params Php::Parameters params
 * @return Php::Value<WSClient> instance
 */
Php::Value WSClient :: set_sequence_key(Php::Parameters &params) {

	auto p = [params] ()->Php::Value {
		Php::Value p = params[0];
		return p.isArray() ? p[WSF_SEQUENCE_KEY] : p;
	};
	Php::Value self(this);

	if(!p().isString()) {
		throw Php::Exception("Unexpected parameter for Sequence key - Expected string" );
	}
	self[WSF_SEQUENCE_KEY] = p();

	return self;
};

/**
 * set rm response time
 *
 * @access public
 * @params Php::Parameters params
 * @return Php::Value<WSClient> instance
 */
Php::Value WSClient :: set_rm_response_time(Php::Parameters &params) {

	auto p = [params] ()->Php::Value {
		Php::Value p = params[0];
		return p.isArray() ? p[WSF_RM_RESPONSE_TIMEOUT] : p;
	};
	Php::Value self(this);

	if(!p().isString()) {
		throw Php::Exception("Unexpected parameter for rm response time - Expected string" );
	}
	self[WSF_RM_RESPONSE_TIMEOUT] = p();

	return self;
};

/**
 * set time out OK
 *
 * @access public
 * @params Php::Parameters params
 * @return Php::Value<WSClient> instance
 */
Php::Value WSClient :: set_timeout(Php::Parameters &params) {

	Php::Value self(this);
	Php::Value p = params[0];

	if(!p.isNumeric()) {
		throw Php::Exception("Unexpected parameter for ws time out - Expected integer" );
	}

	_timeout = p;

	self[WSF_TIMEOUT] = p;

	return self;
};

/**
 * set WSDL
 *
 * @access public
 * @params Php::Parameters params
 * @return Php::Value<WSClient> instance
 */
Php::Value WSClient :: set_wsdl(Php::Parameters &params) {

	auto p = [params] ()->Php::Value {
		Php::Value p = params[0];
		return p.isArray() ? p[WSF_WSDL] : p;
	};
	Php::Value self(this);

	if(!p().isString() && !p().isArray()) {
		throw Php::Exception("Unexpected parameter for WSF_WSDL - Expected string or array" );
	}
	self[WSF_WSDL] = p();

	return self;
};

/**
 * set WSF_CLASSMAP
 *
 * @access public
 * @params Php::Parameters params
 * @return Php::Value<WSClient> instance
 */
Php::Value WSClient :: set_classmap(Php::Parameters &params) {

	auto p = [params] ()->Php::Value {
		Php::Value p = params[0];
		return p.isArray() ? p[WSF_CLASSMAP] : p;
	};
	Php::Value self(this);

	if(!p().isArray()) {
		throw Php::Exception("Unexpected parameter for WSF_Classmap - Expected string or array" );
	}
	self[WSF_CLASSMAP] = p();

	return self;
};

/**
 * set Http auth username ( OK )
 *
 * @access public
 * @params Php::Parameters params
 * @return Php::Value<WSClient> instance
 */
Php::Value WSClient :: set_http_auth_username(Php::Parameters &params) {

	Php::Value self(this);
	Php::Value p = params[0];

	if(!p.isString()) {
		throw Php::Exception("Unexpected parameter for HTTP AUTH Username - Expected string" );
	}
	_http_username = p.stringValue();

	self[WSF_HTTP_AUTH_USERNAME] = p;

	return this;
};

/**
 * set Http auth password ( OK )
 *
 * @access public
 * @params Php::Parameters params
 * @return Php::Value<WSClient> instance
 */
Php::Value WSClient :: set_http_auth_password(Php::Parameters &params) {

	Php::Value self(this);
	Php::Value p = params[0];

	if(!p.isString()) {
		throw Php::Exception("Unexpected parameter for HTTP AUTH password - Expected string" );
	}
	_http_password = p.stringValue();

	self[WSF_HTTP_AUTH_PASSWORD] = p;

	return this;
};

/**
 * set Http auth type OK
 *
 * @access public
 * @params Php::Parameters params
 * @return Php::Value<WSClient> instance
 */
Php::Value WSClient :: set_http_auth_type(Php::Parameters &params) {

	Php::Value self(this);
	Php::Value p = params[0];

	if(!p.isString()) {
		throw Php::Exception("Unexpected parameter for HTTP AUTH type - Expected string" );
	}

	_http_auth_type = p.stringValue();

	self[WSF_HTTP_AUTH_TYPE] = p;

	return this;
};

/**
 * set Proxy auth username OK
 *
 * @access public
 * @params Php::Parameters params
 * @return Php::Value<WSClient> instance
 */
Php::Value WSClient :: set_proxy_auth_username(Php::Parameters &params)
{
	Php::Value self(this);
	Php::Value p = params[0];

	if(!p.isString()) {
		throw Php::Exception("Unexpected parameter for Proxy AUTH Username - Expected string" );
	}
	_proxy_username = p.stringValue();

	self[WSF_PROXY_AUTH_USERNAME] = p;

	return self;
};


/**
 * set Proxy auth password OK
 *
 * @access public
 * @params Php::Parameters params
 * @return Php::Value<WSClient> instance
 */
Php::Value WSClient :: set_proxy_auth_password(Php::Parameters &params)
{
	Php::Value self(this);
	Php::Value p = params[0];

	if(!p.isString()) {
		throw Php::Exception("Unexpected parameter for Proxy AUTH password - Expected string" );
	}
	_proxy_password = p.stringValue();

	self[WSF_PROXY_AUTH_PASSWORD] = p;

	return self;
};

/**
 * set Proxy auth type OK
 *
 * @access public
 * @params Php::Parameters params
 * @return Php::Value<WSClient> instance
 */
Php::Value WSClient :: set_proxy_auth_type(Php::Parameters &params)
{
	Php::Value self(this);
	Php::Value p = params[0];

	if(!p.isString()) {
		throw Php::Exception("Unexpected parameter for Proxy AUTH type - Expected string" );
	}
	_proxy_auth_type = p.stringValue();

	self[WSF_PROXY_AUTH_TYPE] = p;

	return self;
};

/**
 * set http headers
 *
 * @access public
 * @params Php::Parameters params
 * @return Php::Value<WSClient> instance
 */
Php::Value WSClient :: set_http_headers(Php::Parameters &params) {

	auto p = [params] ()->Php::Value {
		Php::Value p = params[0];
		return p.isArray() ? p[WSF_HTTP_HEADERS] : p;
	};
	Php::Value self(this);

	if(!p().isArray()) {
		throw Php::Exception("Unexpected parameter for Http Headers - Expected array" );
	}
	self[WSF_HTTP_HEADERS] = p();

	return self;
};

/**
 * Set a WSMessage
 */
Php::Value WSClient :: set_wsmessage(Php::Parameters &params)
{
	Php::Value p = params[0];

	if(p.isObject() && p.instanceOf("WSMessage"))
    {
		Php::Value self(this);

		_wsmessage = (WSMessage *)p.implementation();

		self["message"] = p;
	}
	else
	{
		throw Php::Exception("Unexpected parameter - Need WSMessage instance");
	}

	return this;
};

/**
 * Set a WSSecurityToken
 */
Php::Value WSClient :: set_wssectoken(Php::Parameters &params)
{
	Php::Value p = params[0];

	if(p.isObject() && p.instanceOf("WSSecurityToken"))
	{
		Php::Value self(this);

		_wssectoken = (WSSecurityToken *)p.implementation();

		self["sec_token"] = p;
	}
	else
	{
		throw Php::Exception("Unexpected parameter - Need WSSecurityToken instance");
	}

	return this;
};

/**
 * Set a WS Policy Object
 */
Php::Value WSClient :: set_wspolicy(Php::Parameters &params)
{
	Php::Value p = params[0];

	if(p.isObject() && p.instanceOf("WSPolicy"))
	{
		Php::Value self(this);

		_wspolicy = (WSPolicy *)p.implementation();

		self["policy"] = p;
	}
	else
	{
		throw Php::Exception("Unexpected parameter - Need WSPolicy instance");
	}

	return this;
}

Php::Value WSClient :: get_wsmessage()
{
	Php::out << _wsmessage->_rest_content_type << endl;

	return Php::Object("WSMessage", _wsmessage);


	//return o;
}

Php::Value WSClient :: get_wssectoken()
{
	return Php::Object("WSSecurityToken", _wssectoken);
}

Php::Value WSClient :: get_wspolicy()
{
	return Php::Object("WSPolicy", _wspolicy);
}

Php::Value WSClient :: get_debug() {

	Php::out << "----- DEBUG -----" << endl;

	return _wsmessage->_fault->get<string>();
}



