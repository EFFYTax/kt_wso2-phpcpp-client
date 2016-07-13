#ifndef WS_CLIENT_H
#define WS_CLIENT_H

#include <phpcpp.h>
#include <iostream>
#include <map>
#include <boost/scoped_array.hpp>
#include <boost/format.hpp>
#include <boost/any.hpp>
#include <axis2_svc_client.h>
#include <axis2_http_transport.h>
#include <axis2_addr.h>
#include <axis2_const.h>
#include <axiom_util.h>
#include <axiom_soap.h>
#include <axutil_env.h>
#include <axis2_const.h>
#include <axis2/axis2_client.h>
#include <ws_const.h>
#include "ws_fault.h"

using namespace std;

class WSClient
		  :public Php::Base {

private:

    /**
     * Shared ptr holding the Axis2Client
     */
    std::shared_ptr<Axis2Client> axis2Client;

    /*
     *
     */
    Php::Object _soap_fault;

public:

	WSClient();

	virtual ~WSClient();

	void __construct();

	void request();

	bool hasMessage();

	bool hasProxy();

	bool hasSSL();

	bool hasHTTPAuth();

	bool hasProxyAuth();

	bool hasSecurity();

	/*
	 * Workaround to know if are facing a soapfault
	 */
	Php::Value hasSoapFault();

	/**
	 * Get the soap fault object or return null
	 */
	Php::Value getSoapFault();

	/**
	 *
	 */
	Php::Value disable_soap();

	/**
	 * set the soap version
	 */
	Php::Value set_soap_version(Php::Parameters &params);

	/**
	 * set http method
	 */
	Php::Value set_http_method(Php::Parameters &params);

	/**
	 * set ws endpoint
	 */
	Php::Value set_endpoint(Php::Parameters &params);

	/**
	 * set transport url
	 */
	Php::Value set_transport_url(Php::Parameters &params);

	/**
	 * set use wsa
	 */
	Php::Value set_use_wsa(Php::Parameters &params);
	/**
	 * Set wsa address
	 */
	Php::Value set_wsa_address(Php::Parameters &params);

	/**
	 * set wsa ref params
	 */
	Php::Value set_wsa_ref_params(Php::Parameters &params);

	/**
	 * set ssl server certificate
	 */
	Php::Value set_server_cert(Php::Parameters &params);

	/**
	 * set ssl client certificate
	 */
	Php::Value set_client_cert(Php::Parameters &params);

	/**
	 * set ssl passphrase
	 */
	Php::Value set_ssl_passphrase(Php::Parameters &params);

	/**
	 * set proxy host
	 */
	Php::Value set_proxy_host(Php::Parameters &params);

	/**
	 * set proxy port
	 */
	Php::Value set_proxy_port(Php::Parameters &params);

	/**
	 * set XOP response
	 */
	Php::Value set_xop_response(Php::Parameters &params);

	/**
	 * set use MTOM
	 */
	Php::Value set_use_mtom(Php::Parameters &params);

	/**
	 * set default attachement content type
	 */
	Php::Value set_default_attachement_content_type(Php::Parameters &params);

	/**
	 * set policy name
	 */
	Php::Value set_policy_name(Php::Parameters &params);

	/**
	 * set usage of ws reliable
	 */
	Php::Value set_ws_reliable(Php::Parameters &params);

	/**
	 * set sequence expiry time
	 */
	Php::Value set_sequence_expiry_time(Php::Parameters &params);

	/**
	 * set contine sequence
	 */
	Php::Value set_contine_sequence(Php::Parameters &params);

	/**
	 * set sequence key
	 */
	Php::Value set_sequence_key(Php::Parameters &params);

	/**
	 * set rm response time
	 */
	Php::Value set_rm_response_time(Php::Parameters &params);

	/**
	 * set ws time out
	 */
	Php::Value set_timeout(Php::Parameters &params);

	/**
	 * set http_auth username
	 */
	Php::Value set_http_auth_username(Php::Parameters &params);

	/**
	 * set http_auth password
	 */
	Php::Value set_http_auth_password(Php::Parameters &params);

	/**
	 * set http auth type
	 */
	Php::Value set_http_auth_type(Php::Parameters &params);

	/**
	 * set proxy auth username
	 */
	Php::Value set_proxy_auth_username(Php::Parameters &params);

	/**
	 * set proxy auth password
	 */
	Php::Value set_proxy_auth_password(Php::Parameters &params);

	/**
	 * set proxy auth type
	 */
	Php::Value set_proxy_auth_type(Php::Parameters &params);

	/**
	 * set http headers
	 */
	Php::Value set_http_headers(Php::Parameters &params);


	/********
	 *
	 */
	Php::Value set_wsmessage(Php::Parameters &params);

	Php::Value set_wssectoken(Php::Parameters &params);

	Php::Value set_wspolicy(Php::Parameters &params);

	Php::Value get_wspolicy();

	Php::Value get_wsmessage();

	Php::Value get_wssectoken();

	Php::Value get_debug();
};

#endif
