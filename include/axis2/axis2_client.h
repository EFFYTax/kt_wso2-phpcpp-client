#ifndef AXIS2_CLIENT_PHP_H
#define AXIS2_CLIENT_PHP_H

#include <iostream>
#include <unistd.h>
#include <axis2/axis2_core.h>
#include <axis2_policy_include.h>
#include <axis2_options.h>
#include <rampart_context.h>
#include <rampart_sec_processed_result.h>
#include <neethi_engine.h>
#include <neethi_options.h>
#include <ws_message.h>
#include <ws_policy.h>
#include <ws_security_token.h>
#include <boost/shared_ptr.hpp>
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

	/*
	 * Structure holding WS-RM options
	 */
	struct ReliableType {

	    double reliable_version            = -1; //1.0 -> 1, 1.1 -> 2
        string reliable_spec_version_str;
        string sequence_key;
        bool   engage_rm                   = false;
        bool   is_engaged_rm               = false;
        long   sequence_expiry_time        = 50000;
	} reliableOpts;

	/*
	 * Structure holding WS-A options
	 */
	struct AddressingType {

	    string use_wsa;
	    bool   is_addressing_engaged  = false;
	    bool   is_addr_action_present = false;
	} addressingOpts;

	/**
	 * Structure holding the transport options
	 */
	struct TransportType {

	    string http_method    = "POST";
        string transport_url;
        string soap_action;
        int    timeout        = 60;   //timeout in seconds - issue w/ SSL
        bool   use_soap       = true;
        double soap_version   = 1.1;
	} tansportOpts;

	/*
	 * Structure holding the rampart options
	 */
	struct RampartOptType {

	    std::vector<char> username ;
	    std::vector<char> password ;
	    std::vector<char> certificate;
	    std::vector<char> receiver_certificate;
	    std::vector<char> private_key;

	    std::unique_ptr<rampart_context_t, std::function<void(rampart_context_t *)>> rampart_ctx {nullptr,
	        [&](rampart_context_t * p){/* Free'd by axutil_allocator_free_impl */}
	    };
	} rampartOpts;

	/**
	 * Structure handling SSL Cert
	 */
	struct SslType {

	    string ssl_server_key_filename;
	    string ssl_client_key_filename;
	    string ssl_passphrase;

	    std::unique_ptr<axutil_property_t, std::function<void(axutil_property_t *)>> ssl_server_key_prop {nullptr,
	        [&](axutil_property_t * p){ /* Free'd by axutil_allocator_free_impl */ }
	    };
	    std::unique_ptr<axutil_property_t, std::function<void(axutil_property_t *)>> ssl_client_key_prop {nullptr,
	        [&](axutil_property_t * p){ /* Free'd by axutil_allocator_free_impl */ }
	    };
	    std::unique_ptr<axutil_property_t, std::function<void(axutil_property_t *)>> passphrase_prop     {nullptr,
	        [&](axutil_property_t * p){ /* Free'd by axutil_allocator_free_impl */ }
	    };
	} sslConf;


	/**
	 * Structure holding http authentication
	 */
	struct HttpAuthType {
	    string http_username;
	    string http_password;
	    string http_auth_type;
	} httpAuth;

	/*
     * Structure holding proxy settings
     */
    struct ProxyType {
        string proxy_host;
        string proxy_port;
        string proxy_username;
        string proxy_password;
        string proxy_auth_type;
    } proxyConf;

	/**
	 * Structure holding a SoapFault
	 */
	struct FaultType {
		string code    = "0";
		string reason  = "";
		string node    = "";
		string details = "";
		string role    = "";
	};

	/**
	 * Structure holding an axis2 module configuration where :
	 *
	 * module : is the module beeing configured
	 * param  : the module param or key to configure
	 * value  : the value for the module to configure
	 *
	 */
	struct ModuleType {
	    std::vector<char> module;
	    std::vector<char> param;
	    std::vector<char> value;
	};

	/**
	 * Structure handling the response
	 */
	struct ResponseType {
	    int status_code = 0;
	    std::unique_ptr<axiom_node_t, std::function<void(axiom_node_t *)>> payload{nullptr,
	        [](axiom_node_t * p){ /* Free'd by axis2 allocator */ }};
	} response;

	/**
	 * c++ ctor
	 */
	Axis2Client(const Axis2Config& configuration);

	/**
	 * c++ dtor
	 */
	virtual ~Axis2Client();

	/*
	 * Create the Axis2 service client
	 */
	void create();

	/**
	 * Set rampart options
	 *
	 * Mainly manage WS-Security to client
	 *
	 * @return void
	 */
	void setRampartOptions();

	/**
	 * Create a WSPolicy
	 *
	 * This need at least a WSPolicy Object.
	 */
	std::unique_ptr<axiom_node_t, std::function<void(axiom_node_t *)>> createPolicy();

	//Big Hack...@see create_header_node
	int _ns_index = 0;

	FaultType handle_soap_fault(axiom_soap_fault_t * soap_fault);
	string get_soap_fault_code(axiom_node_t *code_node, axiom_element_t *code_element);
	string get_soap_fault_reason(axiom_node_t *reason_node, axiom_element_t *reason_element);
	string get_soap_fault_detail(axiom_node_t *detail_node, axiom_element_t *detail_element);

	axiom_node_t            * create_header_node(WSHeader *wsheader, axiom_node_t *parent, axis2_char_t *soap_uri, int soap_version);

	/*
	 * Axis2 Service client
	 *
	 * Note that the lambda will call the axis2_svc_client to free the memory
	 */
	std::unique_ptr<axis2_svc_client, std::function<void (axis2_svc_client *)>> _svc_client{nullptr,
	    [&](axis2_svc_client *p )->void { /* Free'd by Axis2Client::dtor */ }
	};

	/*
	 * Axis2 Client options
	 *
	 * Note that the delete Lambda is empty as axis2_svc_client_free will call axis2_options_free()
	 */
	std::unique_ptr<axis2_options, std::function<void (axis2_options *)>> _client_options{nullptr, [](axis2_options * p){}};

	/**
	 * A ptr to the request payload,
	 *
	 * Note : the delete lambda will free
	 */
	std::unique_ptr<axiom_node_t, std::function<void (axiom_node_t *)>> _request_payload {nullptr, [&](axiom_node_t *p )->void {
	    if(p) axis2_options_set_xml_parser_reset(_client_options.get(), env.get(), 1);
	}};

	/**
	 * Get ptr Axiom Root Element by XML String.
	 */
	virtual std::unique_ptr<axiom_node_t, std::function<void(axiom_node_t *)>> getAxiomRootElementByXMLString(string str);

	/**
	 * Set a Proxy for client
	 *
	 * @return void
	 */
	virtual void setProxy();

	/**
	 * Set Proxy Authentication
	 *
	 * @return void
	 */
	virtual void setProxyAuth();

	/**
	 * Set Http Authentication
	 *
	 * TODO: Document possible alternative methods
	 *
	 * @return void
	 */
	virtual void setHttpAuth();

	/*
	 * Set SSL Authentication
	 *
	 * TODO: Document a bit
	 *
	 * @return void
	 */
	virtual void setSSLAuthentication();

	/*
	 * Set the axis2/c timeout in transport
	 *
	 * @return void
	 */
	virtual void setTimeout();

	/*
	 * Set the axis2/c Soap in transport
	 *
	 * @return void
	 */
	virtual void setSoapVersion();

	/*
	 * Set the axis2/c Http transport
	 *
	 * @return void
	 */
	virtual void setHttpTransport();

	/*
	 * Set the axis2/c endpoint and soap action
	 *
	 * @return void
	 */
	virtual void setEndpointAndSoapAction();

	/*
	 * Set the axis2/c soap headers
	 *
	 * @return void
	 */
	virtual bool setSoapHeaders();

	/**
	 * Set the axis2/c WS-Reliable opts
	 *
	 * @return void
	 */
	virtual void setWSReliableOpts();

	/*
     * set WS-Security client options
     *
     * @return void
     */
    virtual void setWSSecurityOpts();

    /*
     * Set the axis2/c WS-Addressing Options
     *
     * @return void
     */
    virtual void setWSAOpts();

	/*
	 * Get Soap Fault
	 *
	 * @return FaultType
	 */
	virtual FaultType getSoapFault();

	/*
	 * Get Soap Response
	 *
	 * @return string
	 */
	virtual string getSoapResponse();

	/*
	 * Has soap fault
	 *
	 * @return boolean
	 */
	virtual bool hasSoapFault();

	/*
	 * Has Request is WS-Reliable
	 *
	 * @return void
	 */
	virtual bool hasReliable();

	/*
	 * Set Attachment caching (MTOM/XOP)
	 */
	virtual void setAttachmentCaching();

	/*
	 * register module options
	 *
	 * @param module name
	 * @param the parameter for the module
	 * @param the corresponding value
	 */
	virtual void registerModuleOpts(const string module, const string param, const string value);

	/**
	 * WSMessage Object
	 */
	std::shared_ptr<Axis2Message> _wsmessage{nullptr};

	//WSMessage           * _wsmessage = nullptr;
	WSSecurityToken 	* _wssectoken = nullptr;
	WSPolicy 	 		* _wspolicy = nullptr;

	axis2_endpoint_ref_t    * add_endpoint_values_ref(MAP param);

	/**
	 *	Get Options / service client
	 */
	virtual axis2_options_t     * get_options();

	virtual axis2_char_t        * get_soap_version_uri();
	virtual int					  get_soap_version();

	//

};

#endif
