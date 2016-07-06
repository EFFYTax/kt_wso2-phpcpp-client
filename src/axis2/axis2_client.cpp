#include <axis2/axis2_client.h>

/**
 * ctor
 */
Axis2Client :: Axis2Client() : Axis2Core()
{
	_wsmessage 		= nullptr;
	_wspolicy   	= nullptr;
	_wssectoken 	= nullptr;
	_rampart_ctx    = nullptr;

	//Initialise Axis2c service client
	create();

	/**
	 * Set sandesha2 DB
	 * TODO : Move this into the PHP.INI FIle
	 */
	set_module_option("sandesha2", "sandesha2_db", "/tmp/sandesha2_db");
};

/**
 * dtor
 */
Axis2Client :: ~Axis2Client()
{
	axis2_svc_client_free(_svc_client, _env);
};

/**
 * Create the Axis2 service client
 *
 * @access private
 * @return void
 */
void Axis2Client :: create()
{
	_svc_client = axis2_svc_client_create(
		_env,
		_axis2_directory.c_str()
	);

	_client_options = (axis2_options_t *)axis2_svc_client_get_options(_svc_client, _env);
};

/**
 * proxy to get service client
 *
 * @access protected
 * @return axis2_svc_client_t *
 */
axis2_svc_client_t  * Axis2Client :: get_client()
{
	return _svc_client;
};

/**
 * Proxy to the axis2 client options
 *
 * 	@access protected
 * 	@return axis2_options_t *
 */
axis2_options_t * Axis2Client :: get_options()
{
	return  _client_options;
};

/**
 * Get the soap version URI
 *
 * Proxy to the axis2 get soap version uri
 *
 * @access protected
 * @return char *
 */
axis2_char_t * Axis2Client :: get_soap_version_uri()
{
	return (axis2_char_t*)axis2_options_get_soap_version_uri(_client_options, _env);
}

/**
 * Get the soap version
 *
 * Proxy to axis2 get soap version
 *
 * @access protected
 * @return integer
 */
int Axis2Client :: get_soap_version()
{
	return axis2_options_get_soap_version(_client_options, _env);
}

/**
 * Get Axiom Node from XML Payload
 *
 * @access protected
 * @return axiom_node_t *
 */
axiom_node_t * Axis2Client :: get_node_by_xmlstr(string str)
{
	axiom_xml_reader * reader   = get_reader_from_val(str);
	axiom_node_t     * payload  = read_payload(reader);

	return payload ? payload : nullptr;
}

/**
 * Enable attachment caching
 *
 * TODO: Missing implementation
 */
void Axis2Client :: enable_attachment_caching()
{
	//enable_attachment_caching ( global )
	bool caching = false;
	if (_svc_client && caching)
	{
		axis2_svc_ctx_t  *svc_ctx 	= nullptr;
		axis2_conf_ctx_t *conf_ctx 	= nullptr;
		axutil_param_t   *cache_dir = nullptr;
		axis2_conf_t     *conf      = nullptr;

		svc_ctx   = axis2_svc_client_get_svc_ctx(_svc_client, _env);
		conf_ctx  = axis2_svc_ctx_get_conf_ctx(svc_ctx, _env);
		conf 	  = axis2_conf_ctx_get_conf(conf_ctx, _env);
		//cache_dir = axutil_param_create(env, AXIS2_ATTACHMENT_DIR,axutil_strdup(env, "attachment_cache_dir"));
		//axis2_conf_add_param(conf, env, cache_dir);
	}
};

/**
 * Proxy to axis2 set_module_param_option
 *
 * 	@access protected
 * 	@return void
 */
void Axis2Client :: set_module_option(const string module, const string param, const string value)
{
	set_module_param_option(
		get_axis2_char_ptr(module),
		get_axis2_char_ptr(param),
		get_axis2_char_ptr(value),
		_svc_client
	);
}

/**
 * XML Reader - get the raw payload from method param and return a reader
 *
 * @param  Php::Value payload
 * @return axiom_xml_reader_t reader
 */
axiom_xml_reader_t * Axis2Client :: get_reader_from_val(string payload)
{
    axiom_xml_reader_t * reader = nullptr;

    reader = axiom_xml_reader_create_for_memory (
    	_env,
		get_axis2_char_ptr(payload),
		payload.size(),
		AXIS2_UTF_8,
		AXIS2_XML_PARSER_TYPE_BUFFER);

    if (!reader)
    {
    	log("XML Reader creation failed The input string payload is either null or malformed", __FILE__,__LINE__);
		throw Php::Exception( "XML Reader creation failed The input string payload is either null or malformed" );
	}

    return reader;
};

/**
 * Create a payload from reader
 *
 * @param reader
 * @return mixed payload | null
 */
axiom_node_t * Axis2Client :: read_payload (axiom_xml_reader_t * reader) {

    axiom_stax_builder_t *builder  = nullptr;
    axiom_document_t 	 *document = nullptr;
    axiom_node_t 		 *payload  = nullptr;

    builder = axiom_stax_builder_create (_env, reader);

    if (!builder)
    {
        return NULL;
    }
    document = axiom_stax_builder_get_document (builder, _env);

    if (!document)
    {
        return NULL;
    }
    payload = axiom_document_get_root_element (document, _env);

    if (!payload)
    {
        return NULL;
    }
    axiom_document_build_all (document, _env);

    /** free stax builder and associated document only */
    axiom_stax_builder_free_self (builder, _env);

    return payload;
};

/**
 * Register proxy configuration for the client
 *
 * @access protected
 * @return void
 */
void Axis2Client :: register_proxy()
{
	if (!_proxy_host.empty() && !_proxy_port.empty())
	{
		axis2_svc_client_set_proxy (
			_svc_client,
			_env,
			get_axis2_char_ptr(_proxy_host),
			get_axis2_char_ptr(_proxy_port)
		);

		string msg = (boost::format("Setting proxy options %1% -- %2% -- ")
			% _proxy_host
			% _proxy_port ).str();
		log(msg, __FILE__,__LINE__);
	}
}

/**
 * set proxy authentication
 */
void Axis2Client :: register_proxy_auth() {

	if(!_proxy_username.empty() && !_proxy_password.empty())
	{
		if(axis2_options_set_proxy_auth_info(
			_client_options,
			_env,
			get_axis2_char_ptr(_proxy_username),
			get_axis2_char_ptr(_proxy_password),
			get_axis2_char_ptr(_proxy_auth_type)) == AXIS2_SUCCESS)
		{
			log("Success in setting proxy authentication information", __FILE__,__LINE__);
		}
		else
		{
			log("Failed in setting proxy authentication information", __FILE__,__LINE__);
		}
	}
};

/**
 * Enable SSL at service client level this can be
 *
 * @access private
 * @return void
 */
void Axis2Client :: register_ssl()
{
	axutil_property_t * ssl_server_key_prop = nullptr;
	axutil_property_t * ssl_client_key_prop = nullptr;
	axutil_property_t * passphrase_prop     = nullptr;

	if(!_ssl_server_key_filename.empty())
	{
		ssl_server_key_prop = axutil_property_create_with_args(
			_env, 0, 1, 0, axutil_strdup(_env, get_axis2_char_ptr(_ssl_server_key_filename))
		);

		log((boost::format("Setting SSL server certficate ::  %1%") % _ssl_server_key_filename.c_str()).str(), __FILE__,__LINE__);
	}

	if(!_ssl_client_key_filename.empty())
	{
		ssl_client_key_prop = axutil_property_create_with_args (
			_env, 0, 1, 0, axutil_strdup (_env, get_axis2_char_ptr(_ssl_client_key_filename))
		);

		log((boost::format("Setting SSL client certficate ::  %1%") % _ssl_client_key_filename.c_str()).str(), __FILE__,__LINE__);
	}

	if(!_ssl_passphrase.empty())
	{
		passphrase_prop = axutil_property_create_with_args (
			_env, 0, 1, 0, axutil_strdup (_env, get_axis2_char_ptr(_ssl_passphrase))
		);

		log("passphrase found", __FILE__,__LINE__);
	}

	axis2_options_set_property (_client_options, _env, "SERVER_CERT", 	 ssl_server_key_prop);
	axis2_options_set_property (_client_options, _env, "KEY_FILE",    	 ssl_client_key_prop);
	axis2_options_set_property (_client_options, _env, "SSL_PASSPHRASE", passphrase_prop);
};

/**
 * Register HTTP Authentication
 *
 * @access protected
 * @return void
 */
void Axis2Client :: register_http_auth() {

	if(!_http_username.empty() && !_http_password.empty())
	{
		if(axis2_options_set_http_auth_info(
			_client_options,
			_env,
			get_axis2_char_ptr(_http_username),
			get_axis2_char_ptr(_http_password),
			get_axis2_char_ptr(_http_auth_type)) == AXIS2_SUCCESS)
		{
			log("Success in setting HTTP authentication information", __FILE__,__LINE__);
		}
		else
		{
			log("Failed in Setting HTTP authentication information", __FILE__,__LINE__);
		}
	}
};

/**
 * 1. This method
 * 2. HTTP Transport
 * 3. set_endpoint_and_soap_action();
 *
 */
void Axis2Client :: register_soap_version()
{
	if(_use_soap)
	{
		// set soap version
		axis2_options_set_soap_version(_client_options, _env, [this]()->int { return _soap_version == 1.1 ? 1 : 2; }());

		log((boost::format("useSOAP TRUE setting soap version %1%") % _soap_version).str(), __FILE__,__LINE__);
	}
	//enable rest
	else
	{
		axis2_options_set_enable_rest(_client_options, _env, AXIS2_TRUE);
		//*rest_enabled = AXIS2_TRUE;
	}
};

/**
 * Set HTTP Transport
 */
void Axis2Client :: register_http_transport()
{
	/**
	 * default header type is POST,
	 * so only setting the HTTP_METHOD if GET
	 */
	if(!_http_method.empty())
	{
		axutil_property_t *http_method_property = axutil_property_create (_env);

		axutil_property_set_value(
			http_method_property, _env, axutil_strdup (_env, [this]()->const char *{ return _http_method.c_str();}())
		);

		axis2_options_set_property (_client_options, _env, AXIS2_HTTP_METHOD, http_method_property);

		log((boost::format("Setting HTTPMethod %1% property") % _http_method.c_str()).str(), __FILE__,__LINE__);
	}
};

/**
 * Register endpoint and soap action
 */
void Axis2Client :: register_endpoint_and_soap_action()
{
	axis2_endpoint_ref_t *to_epr = nullptr;

	//We must have an endpoint
	if(_wsmessage->hasEndpoint())
	{
		//The endpoint can be a string
		if(_wsmessage->_endpoint->is_string())
		{
			to_epr = axis2_endpoint_ref_create (
				_env, _wsmessage->_endpoint->get<const char *>()
			);
		}
		else
		{
			//This will set endpoint w/ params and metas
			to_epr = add_endpoint_values_ref(
				_wsmessage->_endpoint->get<map<string,vector<string>>>()
			);
		}
	}
	else
		throw Php::Exception("missing endpoint");

	axis2_options_set_to (_client_options, _env, to_epr);

	//Transport URL
	if(!_transport_url.empty())
	{
		axutil_property_t * transport_url = axutil_property_create_with_args(
			_env,0,0,0, get_axis2_char_ptr(_transport_url)
		);

		axis2_options_set_property(_client_options, _env, AXIS2_TRANSPORT_URL, transport_url);

		log((boost::format("Setting WS Transport URL :: %1%") % _transport_url.c_str()).str(), __FILE__,__LINE__);
	}

	//if client_options
	if(_wsmessage->hasAction())
	{
		string action = _wsmessage->_action->get<string>();

		axutil_string_t *action_string = axutil_string_create (_env, get_axis2_char_ptr(action));
		axis2_options_set_soap_action (_client_options, _env, action_string);

		log((boost::format("Setting WS SOAP Action :: %1%") % action.c_str()).str(), __FILE__,__LINE__);
	}
};

/**
 * Set WS-Addressing options
 */
void Axis2Client :: register_wsa_options() {

	log("Setting addressing options", __FILE__,__LINE__);

	if      (_use_wsa == "1.0")			{ log("useWSA true, version 1.0", __FILE__,__LINE__); }
	else if (_use_wsa == "submission") 	{ log("useWSA true, version is submission", __FILE__,__LINE__); }
	else 							  	{ log("useWSA false, disabled", __FILE__,__LINE__); _is_addressing_engaged = false; }

	if(!_use_wsa.empty())
	{
		if(_wsmessage->hasAction())
		{
			axis2_options_set_action (_client_options, _env, _wsmessage->_action->get<const char *>());
			_is_addr_action_present = true;

			log((boost::format("Addressing action is :- %1%")
				% _wsmessage->_action->get<const char *>()).str(), __FILE__,__LINE__);
		}

		//Lambda handling addressing by type
		//----------------------------------
		auto set_addressing =
		[this] (const string type)->bool
		{
			axis2_endpoint_ref_t *epr = nullptr;
			Param *param 			  = _wsmessage->getParamByName(type);

			if(!param) { return false; }

			if(param->is_string())
			{
				epr = axis2_endpoint_ref_create (_env, param->get<const char *>());

				log((boost::format("Addressing %1% is :- %2%") %type.c_str() %param->get<const char *>()).str(), __FILE__,__LINE__);
			}
			else if (param->is_array())
			{
				epr = add_endpoint_values_ref(param->get<map<string, vector<string>>>());

				log((boost::format("Addressing params array for param :: %1%") %type.c_str()).str(), __FILE__,__LINE__);
			}

			if(type == WSF_REPLY_TO) 		{ axis2_options_set_reply_to (_client_options, _env, epr); }
			else if (type == WSF_FAULT_TO)  { axis2_options_set_fault_to (_client_options, _env, epr); }
			else if( type == WSF_FROM) 		{ axis2_options_set_from     (_client_options, _env, epr); }

			return true;
		};
		//----------------------------------
		//End Lambda

		set_addressing(WSF_FAULT_TO);
		set_addressing(WSF_REPLY_TO);
		set_addressing(WSF_FROM);

		if(_wsmessage->_wsa_must_understand)
		{
			axutil_property_t *must_understand_prop =
				axutil_property_create_with_args(
					_env, AXIS2_SCOPE_REQUEST, false, NULL, get_axis2_char_ptr("true")
				);

			if(must_understand_prop)
			{
				axis2_options_set_property(
					_client_options, _env, "addMustUnderstandToAddr", must_understand_prop
				);
			}
		}
	}

	if(!_use_wsa.empty() && _is_addr_action_present == true)
	{
		axis2_svc_client_engage_module (_svc_client, _env, WSF_MODULE_ADDRESSING);

		log("DEBUG ------> Engaged Addressing module", __FILE__,__LINE__);

		if(_use_wsa == "submission")
		{
			axutil_property_t *prop = axutil_property_create_with_args (_env, 0, AXIS2_TRUE,
				0, axutil_strdup (_env, AXIS2_WSA_NAMESPACE_SUBMISSION));

			axis2_options_set_property (_client_options, _env, AXIS2_WSA_VERSION, prop);

			log("Addressing version is submission", __FILE__,__LINE__);
		}

		_is_addressing_engaged = true;
	}
};

/**
 * Timeout
 */
void Axis2Client :: register_timeout()
{
	axis2_options_set_timeout_in_milli_seconds(
		_client_options, _env, 1000 * _timeout
	);
}

/**
 * Registering headers
 */
bool Axis2Client :: register_header()
{
	axis2_char_t *soap_version_uri = get_soap_version_uri();
	int           soap_version     = get_soap_version();

	if(_wsmessage->hasHeaders())
	{
		log("sending headers found", __FILE__,__LINE__);

		axiom_node_t *header_node = nullptr;

		//Index will be use to calculate the ns:
		for(auto &wsheader: _wsmessage->get_headers<std::vector<WSHeader *>>())
		{
			axiom_node_t * header_node = create_header_node( wsheader, nullptr, soap_version_uri, soap_version );
			_ns_index++;

			//DEBUG
			if (header_node)
			{
				axis2_char_t *om_str_out = nullptr;

				om_str_out = axiom_node_to_string (header_node, _env);

				AXIS2_LOG_DEBUG (_env->log, AXIS2_LOG_SI,
				WSF_PHP_LOG_PREFIX "HEADER_NODES ::::::::: \n\t %s \n", om_str_out);

				AXIS2_FREE(_env->allocator, om_str_out);
				om_str_out = NULL;
			}
		}
	}

	return true;
}

/**
 * Create Header node
 */
axiom_node_t * Axis2Client :: create_header_node(WSHeader *wsheader, axiom_node_t *parent, axis2_char_t *soap_uri, int soap_version )
{
	axiom_node_t      * header_node 	= nullptr;
	axiom_namespace_t * header_ns		= nullptr;
	axiom_element_t   * header_element	= nullptr;
	axiom_namespace_t * soap_ns			= nullptr;
	axiom_attribute_t * role_attr       = nullptr;

	if(wsheader->hasNamespace() && wsheader->hasPrefix())
	{
		header_ns = axiom_namespace_create (
			_env,
			get_axis2_char_ptr(wsheader->_ns),
			get_axis2_char_ptr(wsheader->_prefix)
		);
	}
	else if(wsheader->hasNamespace())
	{
		std::string prefix = "ns" + std::to_string(_ns_index);
		header_ns = axiom_namespace_create (
			_env,
			get_axis2_char_ptr(wsheader->_ns),
			get_axis2_char_ptr(prefix));
	}

	header_element = axiom_element_create (
		_env, parent , get_axis2_char_ptr(wsheader->_name), header_ns, &header_node
	);

	if(wsheader->_must_understand)
	{
		log("must understand value is set", __FILE__,__LINE__);

		string must_understand = to_string(wsheader->_must_understand);

		soap_ns = axiom_namespace_create (_env, soap_uri, "soapenv");

		axiom_attribute_t *mu_attr = axiom_attribute_create (
			_env, AXIOM_SOAP_ATTR_MUST_UNDERSTAND, must_understand.c_str(), soap_ns
		);

		axiom_element_add_attribute (header_element, _env, mu_attr, header_node);
	}

	//TODO: -> Move to Header Object as getter
	auto role = [] (int role)->string {
		map<int, string> map = map_list_of
		(0, "http://www.w3.org/2003/05/soap-envelope/role/next")
		(1, "http://www.w3.org/2003/05/soap-envelope/role/none")
		(3, "http://www.w3.org/2003/05/soap-envelope/role/next");
		return map[role]; };

	string roleval = role(wsheader->_role);

	if(!soap_ns)
		soap_ns = axiom_namespace_create (_env, soap_uri, AXIOM_SOAP_DEFAULT_NAMESPACE_PREFIX);

	if (soap_version == AXIOM_SOAP12 )//&& roleval)
	{
		role_attr = axiom_attribute_create (_env, WSF_HEADER_ROLE, roleval.c_str(), soap_ns);
	    axiom_element_add_attribute (header_element, _env, role_attr, header_node);
	}
	else if (soap_version == AXIOM_SOAP11 )// && roleval)
	{
		role_attr = axiom_attribute_create (_env, WSF_HEADER_ACTOR, roleval.c_str(), soap_ns);
		axiom_element_add_attribute (header_element, _env, role_attr, header_node);
	}

	// Header Object
	if(wsheader->hasHeaderObjects())
	{
		for(auto &it: wsheader->get_headers<vector<WSHeader *>>())
		{
			_ns_index++;
			create_header_node(it, header_node, soap_uri, soap_version);
		}
	}
	//Text or xml
	else
	{
		ptree pt;
		axiom_node_t *node = nullptr;
		try
		{
			stringstream xml; xml << wsheader->get_headers<string>();

			read_xml(xml, pt);
			node = get_node_by_xmlstr(wsheader->get_headers<string>());

			if(node)
				axiom_node_add_child(header_node, _env, node);
		}
		catch (...)
		{
			axiom_element_set_text(
				header_element, _env, wsheader->get_headers<string>().c_str(), header_node
			);
		}
	}

	return header_node;
}

/**
 * Register security via WSSecToken and WSPolicy
 *
 * @access protected
 * @return bool
 */
bool Axis2Client :: register_security()
{
	axiom_node_t 			*policy_node 	= nullptr;
	axis2_svc_ctx_t 		*svc_ctx 		= nullptr;
	axis2_conf_ctx_t 		*conf_ctx 		= nullptr;
	axis2_conf_t 			*conf			= nullptr;
	axutil_param_t 			*security_param = nullptr;
	axiom_element_t 		*root_ele 		= nullptr;
	axis2_desc_t 			*desc 			= nullptr;
	axis2_policy_include_t 	*policy_include = nullptr;
	axis2_svc_t 			*svc 			= nullptr;
	neethi_policy_t 		*neethi_policy 	= nullptr;

	if(!_wssectoken)
		return false;

	if(_wspolicy)
	{
		if(_wspolicy->hasXMLPolicy())
		{
			log("POLICY :: XML found", __FILE__,__LINE__);
			policy_node = get_node_by_xmlstr(_wspolicy->_policy);
		}
		else
		{
			policy_node = create_policy();
		}
	}

	_rampart_ctx = rampart_context_create(_env);

	add_rampart_options();

	if(rampart_context_get_custom_tokens(_rampart_ctx, _env) && _wspolicy == NULL)
		return false;


	if(!_wspolicy &&  rampart_context_get_custom_tokens(_rampart_ctx, _env) != NULL)
	{
		neethi_options_t *neethi_options = neethi_options_create (_env);
		policy_node = neethi_options_get_root_node (neethi_options, _env);
	}

	if(policy_node)
	{
		if(axiom_node_get_node_type (policy_node, _env) == AXIOM_ELEMENT)
		{
			root_ele = (axiom_element_t *)axiom_node_get_data_element (policy_node, _env);

			if (root_ele)
			{
				neethi_policy = neethi_engine_get_policy (_env, policy_node, root_ele);
				if (!neethi_policy)
				{
					return false;
				}
				svc = axis2_svc_client_get_svc (_svc_client, _env);
				if (!svc)
				{
					return false;
				}
				desc = axis2_svc_get_base (svc, _env);
				if (!desc)
				{
					return false;
				}
				policy_include = axis2_desc_get_policy_include (desc, _env);
				if (!policy_include)
				{
					return false;
				}
				axis2_policy_include_add_policy_element (policy_include, _env, AXIS2_SERVICE_POLICY, neethi_policy);

				svc_ctx  		= axis2_svc_client_get_svc_ctx (_svc_client, _env);
				conf_ctx 	    = axis2_svc_ctx_get_conf_ctx   (svc_ctx, _env);
				conf 	 		= axis2_conf_ctx_get_conf      (conf_ctx, _env);
				security_param  = axutil_param_create          (_env, get_axis2_char_ptr(string("RampartConfiguration")), _rampart_ctx);

				axis2_conf_add_param (conf, _env, security_param);
			}
		}
	}
	axis2_svc_client_engage_module (_svc_client, _env, WSF_MODULE_SECURITY);

	//DEBUG = 4
	if (policy_node)
	{
		axis2_char_t *om_str_out = nullptr;

		om_str_out = axiom_node_to_string (policy_node, _env);

		AXIS2_LOG_DEBUG (_env->log, AXIS2_LOG_SI,
		WSF_PHP_LOG_PREFIX "creating rampart client outgoing policy node \n\t %s \n", om_str_out);

		AXIS2_FREE(_env->allocator, om_str_out);
		om_str_out = NULL;
	}

	return true;
};

/**
 * Set endpoint and WSA params
 *
 * @access private
 * @return axis2_endpoint_ref_t *
 *
 */
axis2_endpoint_ref_t * Axis2Client :: add_endpoint_values_ref(map<string, vector<string>> param)
{
	axis2_endpoint_ref_t *endpoint_ref = nullptr;
	string address 			  		   = param[WSF_WSA_ADDRESS][0];

	//Lambda to add a reference parameter in the form of an AXIOM node
	//for both metas and params
	auto set_wsa_params =
	[this, &endpoint_ref] ( vector<string> vector, const string &type ) mutable
	{
		if(vector.size() > 0)
		{
			for(auto &xml_str: vector)
			{
				axiom_node_t       *node   = nullptr;
				node = get_node_by_xmlstr(xml_str);

				axis2_endpoint_ref_add_ref_param(endpoint_ref, _env, node);

				log((boost::format("Setting WS-Adressing %1% :: %2%")
					% type.c_str()
					% xml_str.c_str()
				).str(), __FILE__,__LINE__);
			}
		}
	};
	//-- END LAMDA --

	if(address.empty())
		throw Php::Exception("no suitable endpoint in WSMessage - messaging");

	endpoint_ref = axis2_endpoint_ref_create(_env, address.c_str());

	set_wsa_params(param[WSF_WSA_REFERENCE_PARAMETERS], string ("ref_params"));
	set_wsa_params(param[WSF_WSA_METADATA],             string ("ref_metas"));

	return endpoint_ref;
};

/**
 * Set Rampart Options from WSSecToken Object.
 *
 * Only support UsernameToken and Certificate
 *
 * @access private
 * @return boolean
 *
 */
bool Axis2Client :: add_rampart_options()
{
	//Do we have a rampart context
	if(!_rampart_ctx)
	{
		log("RAMPART :: rampart context null -- abort security", __FILE__,__LINE__);
		return false;
	}

	//affect or return false;
	if(!_wssectoken)
	{
		return false;
	}

	log("RAMPART :: Setting values to rampart context ", __FILE__,__LINE__);

	rampart_context_set_ttl(_rampart_ctx, _env, _wssectoken->_ttl);

	//clockSkewBuffer @undocumented from api
	//rampart_context_set_clock_skew_buffer(rampart_context, env, Z_LVAL_PP(token_val));

	log(_wssectoken->_user, __FILE__,__LINE__);

	if(_wssectoken->hasUser())
	{
		rampart_context_set_user(_rampart_ctx, _env, get_axis2_char_ptr(_wssectoken->_user));

		log("RAMPART :: Setting username", __FILE__,__LINE__);
	}

	if(_wssectoken->hasCertificate())
	{
		rampart_context_set_certificate(_rampart_ctx, _env, get_axis2_char_ptr(_wssectoken->_certificate));

		if(rampart_context_set_certificate_type(_rampart_ctx, _env, AXIS2_KEY_TYPE_PEM) == AXIS2_SUCCESS)
		{
			log("RAMPART :: Setting certificate type", __FILE__,__LINE__);
		}
	}

	if(_wssectoken->hasReceiverCertificate())
	{
		rampart_context_set_receiver_certificate(_rampart_ctx, _env, get_axis2_char_ptr(_wssectoken->_receiver_certificate));

		if(rampart_context_set_receiver_certificate_type(_rampart_ctx, _env, AXIS2_KEY_TYPE_PEM) == AXIS2_SUCCESS)
		{
			log("RAMPART :: Setting receiver certificate type", __FILE__,__LINE__);
		}
	}

	if(_wssectoken->hasPrivateKey())
	{
		rampart_context_set_prv_key(_rampart_ctx, _env, get_axis2_char_ptr(_wssectoken->_private_key));

		log("RAMPART :: Setting pvt key", __FILE__,__LINE__);

		if(rampart_context_set_prv_key_type (_rampart_ctx, _env, AXIS2_KEY_TYPE_PEM) == AXIS2_SUCCESS)
		{
			log("RAMPART :: Setting pvt key format", __FILE__,__LINE__);
		}
	}

	if(_wssectoken->hasPassword())
	{
		rampart_context_set_password(_rampart_ctx, _env, get_axis2_char_ptr(_wssectoken->_password));

		log("RAMPART :: Setting password " + _wssectoken->_password, __FILE__,__LINE__);
	}

	//TODO: Implement Password Callback      @wsf_policy.c:622
	//TODO: Implement PCKS12 KeyStore        @wsf_policy.c:638
	//TODO: Implement Replay Detect Callback @wsf_policy.c:643
	//TODO: Implement Enable RDetect Callb   @wsf_policy.c:658
	//TODO: Implement Custom Token           @wsf_policy.c:664

	log("RAMPART :: Rampart Ready", __FILE__,__LINE__);

	return true;
}

/**
 * Create Policy node from WSPolicy Object
 *
 * @access private
 * @return axiom_node_t * - A node reprensenting the policy
 */
axiom_node_t * Axis2Client :: create_policy()
{
	neethi_options_t 	*neethi_options = nullptr;
	axiom_node_t 		*return_node 	= nullptr;

	if(!_wspolicy) return nullptr;

	log("NEETHI :: Creating the policy node", __FILE__,__LINE__);

	neethi_options = neethi_options_create (_env);

	if (neethi_options)
	{
		//Has Timestamp
		if(_wspolicy->hasTimestamp())
		{
			if (neethi_options_set_include_timestamp (neethi_options, _env, AXIS2_TRUE))
				log("NEETHI :: Timestamp_enabled", __FILE__,__LINE__);
		}

		//Has Username Token
		if(_wspolicy->hasUsernameToken())
		{
			if (neethi_options_set_is_username_token (neethi_options, _env, AXIS2_TRUE))
				log("NEETHI :: Token reference_enabled", __FILE__,__LINE__);
		}

		//Has Policy encrypted
		if(_wspolicy->hasEncrypted())
		{
			if (neethi_options_set_encrypt_body (neethi_options, _env, AXIS2_TRUE))
				log("NEETHI :: Encrypt body option enabled", __FILE__,__LINE__);
		}

		if(_wspolicy->hasAlgoSuite())
		{
			if (neethi_options_set_algorithmsuite (neethi_options, _env, get_axis2_char_ptr(_wspolicy->_algorithm_suite)))
				log("NEETHI :: AlgorithmSuite enabled", __FILE__,__LINE__);
		}

		if(_wspolicy->hasSigned())
		{
			if (neethi_options_set_sign_body (neethi_options, _env, AXIS2_TRUE))
				log("NEETHI :: Sign_enabled", __FILE__,__LINE__);
		}

		if(_wspolicy->hasSecurityTokenRef())
		{
			if (neethi_options_set_keyidentifier (neethi_options, _env, get_axis2_char_ptr(_wspolicy->getRampartTokenValue())))
				log("NEETHI :: Token_ref_enabled", __FILE__,__LINE__);

			/**
			 * TODO: To be Implemented
			if(is_server_side == AXIS2_SUCCESS)
			{
				neethi_options_set_server_side(neethi_options, env, is_server_side);
			}
			**/
		}

		if(_wspolicy->hasProtectionOrder())
		{
			if(_wspolicy->_protection_order == "EncryptBeforeSigning")
			{
				if (neethi_options_set_encrypt_before_sign (neethi_options, _env, AXIS2_TRUE))
					log("NEETHI :: Encrypt_before_sign_enabled", __FILE__,__LINE__);

			}
			else if(_wspolicy->_protection_order == "SignBeforeEncrypt")
			{
				if (neethi_options_set_encrypt_before_sign (neethi_options, _env, AXIS2_FALSE))
					log("Sign_before_encrption_enabled", __FILE__,__LINE__);
			}
			else
			{
				log("NEETHI :: Wrong Protection Order", __FILE__,__LINE__);
			}
		}

		if(_wspolicy->hasSecurityBinding())
		{
			if(_wspolicy->_protection_order == "Asymmetric")
			{
				neethi_options_set_binding(neethi_options, _env, get_axis2_char_ptr(string("AsymmetricBinding")));
			}
			else
			{
				neethi_options_set_binding(neethi_options, _env, get_axis2_char_ptr(string("SymmetricBinding")));
			}
		}
	}

	if (neethi_options)
	{
		return_node = neethi_options_get_root_node (neethi_options, _env);
	}

	return return_node;
}

/**
 * Register reliable messaging
 *
 *
 */
void Axis2Client :: register_reliable_messaging()
{
	axutil_property_t *rm_prop 	= nullptr;

	if(_reliable_version > 0)
	{
		rm_prop = axutil_property_create_with_args (_env, 0, 0, 0, get_axis2_char_ptr([this]()->string {
			return _reliable_version == 1 ? "Spec_2005_02" : "Spec_2007_02"; }
		()));

		axis2_options_set_property (_client_options, _env, WSF_SANDESHA2_CLIENT_RM_SPEC_VERSION, rm_prop);

		log("RM version > 0", __FILE__,__LINE__);

		//RM must be engaged
		_engage_rm = true;
	}

	//
	if(has_reliable())
	{
		if(!_is_addressing_engaged)
		{
			//set addressing
			log("DEBUG-------> is_addressing_engaged false - TO BE Implemented", __FILE__,__LINE__);
		}
		axis2_svc_client_engage_module (_svc_client, _env, WSF_MODULE_ADDRESSING);

		log("useWSA not specified, addressing engaged since rm is engaged", __FILE__,__LINE__);

		axis2_svc_client_engage_module (_svc_client, _env, WSF_MODULE_RM);

		_is_engaged_rm = true;

		//set_module_option("sandesha2", "InactivityTimeout", to_string(_sequence_expiry_time));

		//log("sequenceExpiryTime setted");

		//axutil_property_t *seq_property = NULL;
		//sequence_key = axutil_strdup (_env, get_axis2_char_ptr(_sequence_key));

		//Two way single channel
		char *offered_seq_id = NULL;

		axutil_property_t *sequence_property = NULL;
		offered_seq_id 						 = axutil_uuid_gen (_env);
		sequence_property 					 = axutil_property_create (_env);

		axutil_property_set_value (sequence_property, _env, axutil_strdup (_env, offered_seq_id));
		axis2_options_set_property (_client_options, _env, "Sandesha2OfferedSequenceId" , sequence_property);

		log ("Sandesha2OfferedSequenceId is set as property", __FILE__,__LINE__);

		//Test if we defined a TIMEOUT for RM
		/** default timeout value is 5 */
		char * timeout 						= WSF_RM_DEFAULT_RESPONSE_TIMEOUT;
		axutil_property_t *timeout_property = axutil_property_create_with_args (_env, 0, 0, 0, timeout);

		if (timeout_property)
		{
			axis2_options_set_property (_client_options, _env, "time_out", timeout_property);
		}
	}
	//

	//RM is now engaged

	//IF Seq expiry time / InactivityTimeout
	//set_module_option("sandesha2", "InactivityTimeout", "60");

	//log("sequenceExpiryTime is 50");

	//IF sequenceKey
	//axutil_property_t *seq_property = nullptr;
	//sequence_key 					= axutil_strdup (_env, Z_STRVAL_PP (client_tmp));

	if(_reliable_version == 1)
	{
		axutil_property_t *last_msg_prop = axutil_property_create_with_args (
			 _env, 0, 0, 0, get_axis2_char_ptr("true")
		);

		 axis2_options_set_property (
			_client_options, _env, "Sandesha2LastMessage", last_msg_prop
		 );


		//log("Setting Sandesha2LastMessage");
	}



};

/**
 * Has Reliable
 *
 * 1. addressing is engaged by user specifying useWSA and Action
 * 2. addressing is not specified by useWSA but action present then engage addressing
 * 3. if addressing is engaged then engage RM
 */
bool Axis2Client :: has_reliable()
{
	return ((_is_addressing_engaged ||
			(!_is_addressing_engaged && _is_addr_action_present)) && _engage_rm ) ? true : false;
};

/**
 * Has Fault on the last query
 */
bool Axis2Client :: has_soap_fault()
{
	 return axis2_svc_client_get_last_response_has_fault(_svc_client, _env);
};

/**
 * Get Last Response as String format
 *
 * @access protected
 * @return string
 */
string Axis2Client :: get_response_msg()
{
	axiom_xml_writer_t * writer 	    = nullptr;
	axiom_output_t     * om_output 		= nullptr;
	axis2_char_t       * buffer 		= nullptr;

	axiom_soap_envelope_t *soap_envelope = nullptr;

	soap_envelope = axis2_svc_client_get_last_response_soap_envelope (_svc_client, _env);

	writer = axiom_xml_writer_create_for_memory (_env, nullptr, AXIS2_TRUE, 0,
			AXIS2_XML_PARSER_TYPE_BUFFER);

	om_output = axiom_output_create (_env, writer);

	axiom_node_serialize_sub_tree(_response_payload, _env, om_output);

	buffer = (axis2_char_t *) axiom_xml_writer_get_xml (writer, _env);

	string response = get_axis2_char_ptr(buffer);

	axiom_output_free(om_output, _env);

	return response;
}

/**
 *
 */
string Axis2Client::get_soap_fault_detail(axiom_node_t *detail_node, axiom_element_t *detail_element)
{
	axis2_char_t * detail_string 	 = nullptr;
	axiom_node_t * detail_entry_node = nullptr;

	if(detail_node)
	{
		detail_entry_node = axiom_node_get_first_element(detail_node, _env);
		if(detail_entry_node)
		{
			detail_string = axiom_node_to_string(detail_entry_node, _env);

			if(detail_string) { _env->allocator->free_fn(_env->allocator, detail_string); }
		}
	}

	return detail_string;
}

/**
 *
 */
string Axis2Client::get_soap_fault_reason(axiom_node_t *reason_node, axiom_element_t *reason_element)
{
	axis2_char_t *text_value = NULL;

	if([this]()->int { return _soap_version == 1.1 ? 1 : 2; }() == AXIOM_SOAP12)
	{
		axiom_node_t 	* text_node 	= nullptr;
		axiom_element_t * text_element 	= nullptr;

		text_element = axiom_element_get_first_element(reason_element, _env, reason_node, &text_node);
		if(text_element)
		{
			text_value = axiom_element_get_text(text_element, _env, text_node);
		}
	}
	else if([this]()->int { return _soap_version == 1.1 ? 1 : 2; }() == AXIOM_SOAP11)
	{
		text_value = axiom_element_get_text(reason_element, _env, reason_node);
	}

	return text_value;
}

/**
 *
 */
string Axis2Client::get_soap_fault_code(axiom_node_t *code_node, axiom_element_t *code_element)
{
	axiom_node_t    * code_value_node = nullptr;
	axiom_element_t * code_value_ele  = nullptr;
	char 			* code			  = nullptr;

	if([this]()->int { return _soap_version == 1.1 ? 1 : 2; }() == AXIOM_SOAP12)
	{
		code_value_ele = axiom_element_get_first_element(code_element, _env, code_node, &code_value_node);
		code 		   = axiom_element_get_text(code_value_ele, _env, code_value_node);
	}
	else if([this]()->int { return _soap_version == 1.1 ? 1 : 2; }() == AXIOM_SOAP11)
	{
		code 		   = axiom_element_get_text(code_element, _env, code_node);
	}

	return code;
}



/**
 *
 */
Axis2Client::FaultType Axis2Client::handle_soap_fault(axiom_soap_fault_t * soap_fault)
{
	FaultType fault;

	if(soap_fault)
	{
		axiom_node_t 		      * fault_node  = nullptr;
		axiom_soap_fault_code_t   * fault_code	= nullptr;

		fault_node = axiom_soap_fault_get_base_node(soap_fault, _env);

		if(fault_node)
		{
			fault.node = (string)axiom_node_to_string (fault_node, _env);

			axiom_element_t  				* fault_element = nullptr;
			axiom_child_element_iterator_t  * ele_iterator 	= nullptr;

			fault_element = (axiom_element_t *)axiom_node_get_data_element(fault_node, _env);
			ele_iterator  = axiom_element_get_child_elements(fault_element, _env, fault_node);

			if(ele_iterator)
			{
				axiom_node_t     * node    = nullptr;
				axiom_element_t  * element = nullptr;

				while(axiom_child_element_iterator_has_next(ele_iterator, _env))
				{
					node = axiom_child_element_iterator_next(ele_iterator, _env);
					if(node)
					{
						element = (axiom_element_t *) axiom_node_get_data_element(node, _env);

						if(element)
						{
							char * localname = nullptr;
							localname        = axiom_element_get_localname(element, _env);

							if(localname)
							{
								if      (std::strcmp(localname, AXIOM_SOAP12_SOAP_FAULT_CODE_LOCAL_NAME)   == 0) { fault.code   = get_soap_fault_code(node, element);   }
								else if (std::strcmp(localname, AXIOM_SOAP11_SOAP_FAULT_CODE_LOCAL_NAME)   == 0) { fault.code   = get_soap_fault_code(node, element);   }
								else if (std::strcmp(localname, AXIOM_SOAP12_SOAP_FAULT_REASON_LOCAL_NAME) == 0) { fault.reason = get_soap_fault_reason(node, element); }
								else if (std::strcmp(localname, AXIOM_SOAP11_SOAP_FAULT_STRING_LOCAL_NAME) == 0) { fault.reason = get_soap_fault_reason(node, element); }
								else if (std::strcmp(localname, AXIOM_SOAP12_SOAP_FAULT_DETAIL_LOCAL_NAME) == 0 ||
										 std::strcmp(localname, AXIOM_SOAP11_SOAP_FAULT_DETAIL_LOCAL_NAME)) 	 { fault.details = get_soap_fault_reason(node, element); }
								else if (std::strcmp(localname, AXIOM_SOAP12_SOAP_FAULT_ROLE_LOCAL_NAME)   == 0 ||
										 std::strcmp(localname, AXIOM_SOAP11_SOAP_FAULT_ACTOR_LOCAL_NAME)) 	     { fault.details = get_soap_fault_reason(node, element); }

							}
						}
					}
				}//
			}
		}
	}

	return fault;
}

/**
 * Get the latest fault message
 *
 * @access protected
 * @return string
 */
Axis2Client::FaultType Axis2Client :: get_soap_fault_msg()
{
	FaultType                 fault;
	axiom_soap_envelope_t   * soap_envelope   = nullptr;
	axiom_soap_body_t       * soap_body       = nullptr;
	axiom_soap_fault_t      * soap_fault 	  = nullptr;
	axis2_char_t 	        * res_text 	      = nullptr;

	soap_envelope = axis2_svc_client_get_last_response_soap_envelope (_svc_client, _env);

	if(soap_envelope)
	{
		soap_body  = axiom_soap_envelope_get_body (soap_envelope, _env);

		if(soap_body)
		{
			soap_fault = axiom_soap_body_get_fault (soap_body, _env);

			fault = handle_soap_fault(soap_fault);
		}
	}

	log("Fault Node "    + fault.node,     __FILE__,__LINE__);
	log("Fault code "    + fault.code,     __FILE__,__LINE__);
	log("Fault reason "  + fault.reason,   __FILE__,__LINE__);
	log("Fault details " + fault.details,  __FILE__,__LINE__);
	log("Fault role "    + fault.role,     __FILE__,__LINE__);

	return fault;
}
