#ifndef AXIS2_CLIENT_PHP_H
#define AXIS2_CLIENT_PHP_H

#include <iostream>
#include <unistd.h>
#include <axis2/axis2_core.h>
#include <axis2_policy_include.h>
#include <rampart_context.h>
#include <rampart_sec_processed_result.h>
#include <neethi_engine.h>
#include <neethi_options.h>
#include <ws_message.h>
#include <ws_policy.h>
#include <ws_security_token.h>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ptree.hpp>

using namespace std;
using namespace boost;
using namespace boost::assign;
using namespace boost::property_tree;

class Axis2Client
		:public Axis2Core {

	typedef map<string, vector<string>> MAP;

public:
	struct FaultType {
		string code    = "0";
		string reason  = "";
		string node    = "";
		string details = "";
		string role    = "";
	};

public:

	Axis2Client();

	virtual ~Axis2Client();

private:

	void create();
	void enable_attachment_caching();



	axis2_endpoint_ref_t	* add_endpoint_values_ref(MAP param);
	bool                   	  add_rampart_options();

	axiom_node_t 		 	* create_policy();
	axiom_node_t         	* create_header_node(WSHeader *wsheader, axiom_node_t *parent, axis2_char_t *soap_uri, int soap_version);

	//Big Hack...@see create_header_node
	int _ns_index = 0;

	FaultType handle_soap_fault(axiom_soap_fault_t * soap_fault);
	string get_soap_fault_code(axiom_node_t *code_node, axiom_element_t *code_element);
	string get_soap_fault_reason(axiom_node_t *reason_node, axiom_element_t *reason_element);
	string get_soap_fault_detail(axiom_node_t *detail_node, axiom_element_t *detail_element);

protected:

	//service client
	axis2_svc_client_t  * _svc_client;

	//client options
	axis2_options_t     * _client_options;
	//rampart context
	rampart_context_t 	* _rampart_ctx;

	//Request response
	axiom_node_t		* _request_payload    = nullptr;
	axiom_node_t       	* _response_payload   = nullptr;

	//Proxy
	string _proxy_host;
	string _proxy_port;
	string _proxy_username;
	string _proxy_password;
	string _proxy_auth_type;

	//SSL
	string _ssl_server_key_filename;
	string _ssl_client_key_filename;
	string _ssl_passphrase;

	//HTTP Auth
	string _http_username;
	string _http_password;
	string _http_auth_type;
	string _http_method = "POST";

	string _transport_url;
	string _use_wsa;
	string _soap_action;

	//timeout in seconds - issue w/ SSL
	int    _timeout = 60;

	//
	bool   _use_soap 	  		  		= true;
	bool   _is_addressing_engaged  		= false;
	bool   _is_addr_action_present  	= false;
	double _soap_version  		  		= 1.1;

	double _reliable_version          	= -1; //1.0 -> 1, 1.1 -> 2
	string _reliable_spec_version_str;
	bool   _engage_rm 				  	= false;
	bool   _is_engaged_rm             	= false;
	long   _sequence_expiry_time        = 50000;
	string _sequence_key;

	//
	WSMessage  			* _wsmessage;
	WSSecurityToken 	* _wssectoken;
	WSPolicy 	 		* _wspolicy;

	//
	virtual void set_module_option(const string module, const string param, const string value);
	virtual void register_proxy();
	virtual void register_proxy_auth();
	virtual void register_ssl();
	virtual void register_http_auth();
	virtual void register_timeout();

	virtual void register_soap_version();
	virtual void register_http_transport();
	virtual void register_endpoint_and_soap_action();
	virtual void register_wsa_options();
	virtual bool register_security();
	virtual bool register_header();
	virtual void register_reliable_messaging();

	/**
	 * Get Reader / Read payload
	 */
	virtual axiom_xml_reader_t  * get_reader_from_val(string payload);
	virtual axiom_node_t        * read_payload (axiom_xml_reader_t * reader);
	virtual axiom_node_t        * get_node_by_xmlstr(string str);

	/**
	 *	Get Options / service client
	 */
	virtual axis2_options_t     * get_options();
	virtual axis2_svc_client_t  * get_client();
	virtual axis2_char_t        * get_soap_version_uri();
	virtual int					  get_soap_version();

	//
	virtual bool  				  has_reliable();
	virtual bool       			  has_soap_fault();

	//
	virtual FaultType  	  		  get_soap_fault_msg();
	virtual string                get_response_msg();
};

#endif
