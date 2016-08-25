#include "ws_client.h"

/**
 * WSClient c++ ctor
 */
WSClient::WSClient() {

    try {

        Axis2Config config;

        config.home_folder = Php::ini_get("ktwso2.axis2_directory").stringValue();
        config.log_level   = Php::ini_get("ktwso2.log_level").numericValue();
        config.log_file    = Php::ini_get("ktwso2.log_filename").stringValue();
        config.log_path    = Php::ini_get("ktwso2.log_path").stringValue();

        axis2Client = std::make_shared<Axis2Client>(config);

    } catch(std::exception& e) {
        throw Php::Exception(e.what());
    }
};

/**
 * WSClient c++ dtor
 */
WSClient :: ~WSClient(){};

/**
 * WSClient PHP __ctor
 *
 * {{{ proto void WSClient::__construct(string uri[, array options])
 */
void WSClient :: __construct(){};

/**
 * Check if the WSClient has a WSMessage instance
 *
 * @access protected
 * @return bool
 */
bool WSClient :: hasMessage()
{
    return (axis2Client->_wsmessage) ? true : false;
};

/**
 * Check if the WSClient has a proxy configuration registered
 *
 * @access protected
 * @return bool
 */
bool WSClient :: hasProxy()
{
    return ( !axis2Client->proxyConf.proxy_host.empty() && !axis2Client->proxyConf.proxy_port.empty() ) ? true : false;
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
    return (hasProxy() && !axis2Client->proxyConf.proxy_username.empty() && !axis2Client->proxyConf.proxy_password.empty()) ? true : false;
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
    return (!axis2Client->sslConf.ssl_client_key_filename.empty() || !axis2Client->sslConf.ssl_server_key_filename.empty()) ? true : false;
}

/**
 * Check if the WSClient has a HTTP Authentication registered
 *
 * @access protected
 * @return bool
 */
bool WSClient :: hasHTTPAuth()
{
    return (!axis2Client->httpAuth.http_username.empty() && !axis2Client->httpAuth.http_password.empty()) ? true : false;
}

/**
 * Check if the WSClient has a WSSecurityToken configuration registered
 *
 * @access protected
 * @return bool
 */
bool WSClient :: hasSecurity()
{
    return (axis2Client->_wssectoken) ? true : false;
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

    if(!axis2Client->_wsmessage) {
        throw Php::Exception("Need a WSMessage Instance");
    }

    /**
     * For a unknown reason, the service client could not be re-used and httpclient crash
     * when calling axis2_svc_client_send_receive with the same svc_client.
     * Crash occurs somewhere in soap_builder.c while building the soap_env.
     * ( while next - soap_builder.c:265 ).
     *
     * Someone faced the same issue.
     *
     * http://mail-archives.apache.org/mod_mbox/axis-c-dev/200608.mbox/%3C0DD39110AD1C434C92A686A6F8995D604B1E3A@SRV-MAIL.net.billing.ru%3E
     *
     * I created a JIRA... maybe someone will answer...
     *
     * https://issues.apache.org/jira/browse/AXIS2C-1686
     */
    axis2Client->createServiceClient();

    axis2Client->log((boost::format("ktws::payload is ready :: %1%")
        % axis2Client->_wsmessage->getPayload()->get<string>()).str(), __FILE__,__LINE__);

    axis2Client->_request_payload.reset(axis2Client->getAxiomByXMLString(axis2Client->_wsmessage->getPayload()->get<string>()));

    if (!axis2Client->_request_payload)
    {
        axis2Client->log("request payload node is null", __FILE__,__LINE__);

        if(!axis2Client->tansportOpts.use_soap)
            throw Php::Exception( "request payload node is null" );
    }

    //Configure Axis2/c and relevant modules
    try {

        if(hasProxy())      axis2Client->setProxy();
        if(hasProxyAuth())  axis2Client->setProxyAuth();
        if(hasSSL())        axis2Client->setSSLAuthentication();
        if(hasHTTPAuth())   axis2Client->setHttpAuth();
        if(hasSecurity())   axis2Client->setWSSecurityOpts();

        axis2Client->setSoapVersion();
        axis2Client->setHttpTransport();
        axis2Client->setTransportUrl();
        axis2Client->setEndpointAndSoapAction();
        axis2Client->setWSAOpts();
        axis2Client->setSoapHeaders();
        axis2Client->setTimeout();
        axis2Client->setWSReliableOpts();

        axis2Client->response.payload.reset(axis2_svc_client_send_receive (
                axis2Client->_svc_client.get(), axis2Client->env.get(), axis2Client->_request_payload.get()));

    } catch(const std::exception &e) {
        throw Php::Exception(e.what());
    }

    /*
     * TODO: Implement WS-R / Oneway
     * if(_is_engaged_rm){}
     */
    axis2Client->response.status_code =
            axis2_svc_client_get_http_status_code(axis2Client->_svc_client.get(),
                    axis2Client->env.get());

    //Response code and errors
    std::string     str_status_code = std::to_string(axis2Client->response.status_code);
    string          error_msg       = axutil_error_get_message(axis2Client->env.get()->error);

    axis2Client->log("http status code :: " + str_status_code, __FILE__,__LINE__);

    //REST
    if(!axis2Client->tansportOpts.use_soap)
    {
        if(axis2Client->response.status_code != 200 && axis2Client->response.status_code != 202)
        {
            throw Php::Exception("REST Fault");
        }
    }

    //The SOAP Request is faulty
    else if(axis2Client->hasSoapFault())
    {
        Axis2Client::FaultType fault = axis2Client->getSoapFault();

        //Ctor must have a other constructor for this... just a bit lazzy
        WSFault * soapFault = new WSFault();
        soapFault->_code    = fault.code;
        soapFault->_details = fault.details;
        soapFault->_node    = fault.node;
        soapFault->_reason  = fault.reason;
        soapFault->_role    = fault.role;

        _soap_fault = Php::Object("KTWS\\WSFault", soapFault);

        //Strong limitation of PHP-CPP which is not yet able to throw custom Exception.
        //As a workaround just use standard \Exception with the response payload
        //Alternatively a WSFault object is hydrated and can be accessed through a
        //KTWS\WSClient::getSoapFault() as proxy
        throw Php::Exception( fault.node, axis2Client->response.status_code );
    }

    //Empty response
    else if(!axis2Client->response.payload)
    {
        axis2Client->log("Issue", __FILE__,__LINE__);

        throw Php::Exception("Issue while requesting, no response received");
    }

    //Response
    else
    {
        axis2Client->_wsmessage->_response = axis2Client->getSoapResponse();
    }

    axis2Client->destroyServiceClient();
};

/**
 * This is a workaround... lack of custom exception in PHP-CPP
 */
Php::Value WSClient :: hasSoapFault()
{
    Php::Value self(this);

    return axis2Client->hasSoapFault();
}

/**
 * Get the soap fault
 */
Php::Value WSClient :: getSoapFault()
{
    return _soap_fault;
}

/**
 * Disable soap protocol meaning using rest
 *
 * @access public
 * @return Php::Value<WSClient> instance
 */
Php::Value WSClient :: disable_soap()
{
    Php::Value self(this);

    axis2Client->tansportOpts.use_soap = false;

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
        axis2Client->tansportOpts.soap_version = p.floatValue();

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
            v == "POST"   || v == "post" ||
            v == "GET"    || v == "get"  ||
            v == "PUT"    || v == "put"  ||
            v == "DELETE" || v == "delete"
    ))
    {
        axis2Client->tansportOpts.http_method = v;
    }
    else
        throw Php::Exception("Unexpected parameter for HTTP Method - Expected GET/POST/PUT/DELETE");

    self[WSF_HTTP_METHOD] = v;

    return self;
};

/**
 * Set transport URL
 *
 * @access public
 * @params Php::Parameters params
 * @return Php::Value<WSClient> instance
 */
Php::Value WSClient :: set_transport_url(Php::Parameters &params) {

    Php::Value self(this);
    Php::Value p = params[0];

    if(!p.isString())
        throw Php::Exception("Unexpected parameter for WSF Transport URL - Expected string");

    axis2Client->tansportOpts.transport_url = p.stringValue();

    self["transportURL"] = p;

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

    axis2Client->addressingOpts.use_wsa = p.stringValue();

    self["useWSA"] = axis2Client->addressingOpts.use_wsa;

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

    Php::Value self(this);
    Php::Value p = params[0];

    if(!p.isString() && !p.isArray()) {
        throw Php::Exception("Unexpected parameter for WSA Address - Expected string or array");
    }
    self[WSF_WSA_ADDRESS] = p;

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

    if(p.isString() && !axis2Client->isFileExist(p))
    {
        throw Php::Exception("The server certificate supplied was not found on system" );
    }
    else if(!p.isString())
    {
        throw Php::Exception("Unexpected parameter for SSL Server Cert - Expected string" );
    }

    axis2Client->sslConf.ssl_server_key_filename = p.stringValue();

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

    if(p.isString() && !axis2Client->isFileExist(p))
    {
        throw Php::Exception("The client certificate supplied was not found on system" );
    }
    else if(!p.isString())
    {
        throw Php::Exception("Unexpected parameter for SSL Client Cert - Expected string" );
    }

    axis2Client->sslConf.ssl_client_key_filename = p.stringValue();

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

    axis2Client->sslConf.ssl_passphrase = p.stringValue();

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
    axis2Client->proxyConf.proxy_host = p.stringValue();//

    self["proxyHost"] = p;

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
    axis2Client->proxyConf.proxy_port = p.stringValue();

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

    if((!p().isObject() && !p().instanceOf("KTWS\\WSPolicy")) || !p().isArray()) {
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
        axis2Client->reliableOpts.reliable_version = 1;
    }
    else if(p == 1.1)
    {
        axis2Client->reliableOpts.reliable_version = 2;
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
    axis2Client->tansportOpts.timeout = p;

    self[WSF_TIMEOUT] = p;

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
    axis2Client->httpAuth.http_username = p.stringValue();

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
    axis2Client->httpAuth.http_password = p.stringValue();

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
    axis2Client->httpAuth.http_auth_type = p.stringValue();

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
    axis2Client->proxyConf.proxy_username = p.stringValue();

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
    axis2Client->proxyConf.proxy_password = p.stringValue();

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
    axis2Client->proxyConf.proxy_auth_type = p.stringValue();

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

    if(p.isObject() && p.instanceOf("KTWS\\WSMessage"))
    {
        Php::Value self(this);

        WSMessage * wsmessage = (WSMessage*) p.implementation();

        axis2Client->_wsmessage = wsmessage->getAxis2Message();

        self["ws_message"] = p;
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

    if(p.isObject() && p.instanceOf("KTWS\\WSSecurityToken"))
    {
        Php::Value self(this);

        axis2Client->_wssectoken = (WSSecurityToken *)p.implementation();

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

    if(p.isObject() && p.instanceOf("KTWS\\WSPolicy"))
    {
        Php::Value self(this);

        axis2Client->_wspolicy = (WSPolicy *)p.implementation();

        self["policy"] = p;
    }
    else
    {
        throw Php::Exception("Unexpected parameter - Need WSPolicy instance");
    }

    return this;
}

/**
 *
 */
Php::Value WSClient :: get_wsmessage()
{
    Php::Value self(this);

    return self["ws_message"];

    //return Php::Object("KTWS\\WSMessage", axis2Client->_wsmessage);
}

/**
 *
 */
Php::Value WSClient :: get_wssectoken()
{
    return Php::Object("WSSecurityToken", axis2Client->_wssectoken);
}

/**
 *
 */
Php::Value WSClient :: get_wspolicy()
{
    return Php::Object("WSPolicy", axis2Client->_wspolicy);
}

/**
 *
 */
Php::Value WSClient :: get_debug() {

    Php::out << "----- DEBUG -----" << endl;

    return axis2Client->_wsmessage->getFault()->get<string>();
}
