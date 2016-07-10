#include <axis2/axis2_client.h>

/**
 * ctor
 */
Axis2Client :: Axis2Client(const Axis2Config& config)
: Axis2Core(config)
{
	//Initialise Axis2c service client
	try {
	    create();
        /**
         * Set sandesha2 DB
         * TODO : Move this into the PHP.INI FIle
         */
        registerModuleOpts("sandesha2", "sandesha2_db", "/tmp/sandesha2_db" );
	} catch(...) {
	    //
	}
};

/**
 * dtor
 */
Axis2Client :: ~Axis2Client()
{
	axis2_svc_client_free(_svc_client.get(), env.get());
};

/**
 * Create the Axis2 service client
 *
 * @access private
 * @return void
 */
void Axis2Client :: create()
{
	_svc_client.reset(axis2_svc_client_create(
	        env.get(),
	        a2config.home_folder.c_str())
	);

	if(!_svc_client.get())
	    throw std::invalid_argument("Could not init the axis2/c service client");

	_client_options.reset((axis2_options_t *)axis2_svc_client_get_options(
	    _svc_client.get(),
	    env.get()));

	if(!_client_options)
	    throw std::invalid_argument("Could not init the axis2/c client options");
};

/**
 * Proxy to the axis2 client options
 *
 * 	@access protected
 * 	@return axis2_options_t *
 */
axis2_options_t * Axis2Client :: get_options()
{
	return  _client_options.get();
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
	return (axis2_char_t*)axis2_options_get_soap_version_uri(_client_options.get(), env.get());
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
	return axis2_options_get_soap_version(_client_options.get(), env.get());
}

/**
 * Get Axiom Node from XML string
 *
 * @access protected
 * @return axiom_node_t *
 */ //get_node_by_xmlstr
std::unique_ptr<axiom_node_t, std::function<void(axiom_node_t *)>> Axis2Client :: getAxiomRootElementByXMLString(string str_payload)
{
    std::unique_ptr<axiom_xml_reader_t, std::function<void(axiom_xml_reader *)>> reader {nullptr,
        [&](axiom_xml_reader_t *p){ /* Handled by axiom_stax_builder_free_self() */ }
    };

    //Builder, Lambda free stax builder and associated document only
    std::unique_ptr<axiom_stax_builder_t, std::function<void(axiom_stax_builder_t *)>> builder {nullptr,
        [&](axiom_stax_builder_t *p){ axiom_stax_builder_free_self (p, env.get()); }
    };

    std::unique_ptr<axiom_document_t, std::function<void(axiom_document_t *)>> document {nullptr,
        [&](axiom_document_t *p ){ /* Handled by axiom_stax_builder_free_self */ }
    };

    std::unique_ptr<axiom_node_t, std::function<void(axiom_node_t *)>> ax_payload{nullptr,
        [&](axiom_node_t *p ){ /* Free'd by axutil_allocator_free_impl */ }
    };

    std::vector<char> ptr_payload(str_payload.data(), str_payload.data() + str_payload.size() + 1u);

    //Init the reader
    reader.reset(axiom_xml_reader_create_for_memory(env.get(),
            ptr_payload.data(), str_payload.size(),
            AXIS2_UTF_8, AXIS2_XML_PARSER_TYPE_BUFFER));

    if (!reader) {
        log("Axiom xml reader failed", __FILE__, __LINE__);
        throw std::invalid_argument("Axiom xml reader failed");
    }
    //Init axiom builder
    builder.reset(axiom_stax_builder_create(env.get(), reader.get()));

    if (!builder) {
        log("Axiom statx builder failed", __FILE__, __LINE__);
        throw std::invalid_argument("Axiom statx builder failed");
    }
    document.reset(axiom_stax_builder_get_document(builder.get(), env.get()));

    if (!document) {
        log("Axiom statx document failed", __FILE__, __LINE__);
        throw std::invalid_argument("Axiom statx document failed");
    }
    ax_payload.reset(axiom_document_get_root_element(document.get(), env.get()));

    if (!ax_payload) {
        log("Axiom get root document failed", __FILE__, __LINE__);
        throw std::invalid_argument("Axiom get root document failed");
    }
    axiom_document_build_all(document.get(), env.get());

    return ax_payload;
}

/**
 * Enable attachment caching
 *
 * TODO: Missing implementation
 */
void Axis2Client :: setAttachmentCaching()
{
	//enable_attachment_caching ( global )
	bool caching = false;
	if (_svc_client.get() && caching)
	{
		axis2_svc_ctx_t  *svc_ctx 	= nullptr;
		axis2_conf_ctx_t *conf_ctx 	= nullptr;
		axutil_param_t   *cache_dir = nullptr;
		axis2_conf_t     *conf      = nullptr;

		svc_ctx   = axis2_svc_client_get_svc_ctx(_svc_client.get(), env.get());
		conf_ctx  = axis2_svc_ctx_get_conf_ctx(svc_ctx, env.get());
		conf 	  = axis2_conf_ctx_get_conf(conf_ctx, env.get());
		//cache_dir = axutil_param_create(env, AXIS2_ATTACHMENT_DIR,axutil_strdup(env, "attachment_cache_dir"));
		//axis2_conf_add_param(conf, env, cache_dir);
	}
};

/*
 * Set parameters for a given Axis2c module
 *
 * @access private
 *
 * @param module_name, the module name ( i.e sandesha2 )
 * @param module_option, the module option to modify ( key )
 * @param module_option_value, the module option value ( val )
 *
 * Return AXIS2_SUCESS | AXIS2_FAILLURE
 */
void Axis2Client :: registerModuleOpts(const string mod_name, const string mod_param, const string mod_value)
{
    ModuleType            mod;
    axis2_conf_ctx_t    * conf_ctx       = nullptr;
    axis2_svc_ctx_t     * svc_ctx        = nullptr;
    axis2_module_desc_t * module_desc    = nullptr;
    axis2_conf_t        * conf           = nullptr;
    axutil_qname_t      * module_qname   = nullptr;
    axutil_param_t      * param          = nullptr;

    //Axis2/c API Expect raw pointer
    mod.module.resize(mod_name.size() + 1);
    mod.param.resize(mod_param.size() + 1);
    mod.value.resize(mod_value.size() + 1);

    std::copy(mod_name.c_str(),   mod_name.c_str()  +  mod_name.length()   + 1,  mod.module.begin());
    std::copy(mod_param.c_str(),  mod_param.c_str() +  mod_param.length()  + 1,  mod.param.begin());
    std::copy(mod_value.c_str(),  mod_value.c_str() +  mod_value.length()  + 1,  mod.value.begin());

    //Axis2/c Module configuration
    module_qname = axutil_qname_create          (env.get(), mod.module.data(), nullptr, nullptr);
    svc_ctx      = axis2_svc_client_get_svc_ctx (_svc_client.get(), env.get());
    conf_ctx     = axis2_svc_ctx_get_conf_ctx   (svc_ctx, env.get());
    conf         = axis2_conf_ctx_get_conf      (conf_ctx, env.get());
    module_desc  = axis2_conf_get_module        (conf, env.get(), module_qname);

    if (!module_desc)
    {
        log("module could not be configured", __FILE__,__LINE__);
        throw std::invalid_argument("Issue in set module param option (module_desc)");
    }
    param = axis2_module_desc_get_param (module_desc, env.get(), mod.param.data());

    //@see module.xml for sandesha2 ( db_parameter )
    if (!param)
    {
        log("module could not be configured", __FILE__,__LINE__);
        throw std::invalid_argument("Issue in set module param option (param)");
    }
    axutil_param_set_value (param, env.get(), axutil_strdup (env.get(), mod.value.data()));

    log((boost::format("Setting %1% module param %2% to %3%")
        % mod_name
        % mod_param
        % mod_value).str(), __FILE__,__LINE__);

    axutil_qname_free (module_qname, env.get());
}

/**
 * Register proxy configuration for the client
 *
 * @access protected
 * @return void
 */
void Axis2Client :: setProxy()
{
	if (!proxyConf.proxy_host.empty() && !proxyConf.proxy_port.empty())
	{
	    //set proxy need mutable char ptr
	    std::vector<char> proxy_host(proxy_host.data(), proxy_host.data() + proxy_host.size() + 1u);
	    std::vector<char> proxy_port(proxy_port.data(), proxy_port.data() + proxy_port.size() + 1u);

		axis2_svc_client_set_proxy (
			_svc_client.get(),
			env.get(),
			proxy_host.data(),
			proxy_port.data()
		);

		string msg = (boost::format("Setting proxy options %1% -- %2% -- ")
			% proxyConf.proxy_host
			% proxyConf.proxy_port ).str();
		log(msg, __FILE__,__LINE__);
	}
}

/**
 * set proxy authentication
 */
void Axis2Client :: setProxyAuth() {

	if(!proxyConf.proxy_username.empty() && !proxyConf.proxy_password.empty())
	{
		if(axis2_options_set_proxy_auth_info(
			_client_options.get(),
			env.get(),
			proxyConf.proxy_username.c_str(),
			proxyConf.proxy_password.c_str(),
			proxyConf.proxy_auth_type.c_str()) == AXIS2_SUCCESS)
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
void Axis2Client :: setSSLAuthentication()
{
	if(!sslConf.ssl_server_key_filename.empty())
	{
	    sslConf.ssl_server_key_prop.reset(axutil_property_create_with_args(
			env.get(), 0, 1, 0, axutil_strdup(env.get(), sslConf.ssl_server_key_filename.c_str())
		));
		log((boost::format("Setting SSL server certficate ::  %1%") % sslConf.ssl_server_key_filename.c_str()).str(), __FILE__,__LINE__);
	}

	if(!sslConf.ssl_client_key_filename.empty())
	{
	    sslConf.ssl_client_key_prop.reset(axutil_property_create_with_args (
			env.get(), 0, 1, 0, axutil_strdup (env.get(), sslConf.ssl_client_key_filename.c_str())
		));
		log((boost::format("Setting SSL client certficate ::  %1%") % sslConf.ssl_client_key_filename.c_str()).str(), __FILE__,__LINE__);
	}

	if(!sslConf.ssl_passphrase.empty())
	{
	    sslConf.passphrase_prop.reset(axutil_property_create_with_args (
			env.get(), 0, 1, 0, axutil_strdup (env.get(), sslConf.ssl_passphrase.c_str())
		));
		log("passphrase found", __FILE__,__LINE__);
	}

	axis2_options_set_property (_client_options.get(), env.get(), "SERVER_CERT", 	 sslConf.ssl_server_key_prop.get());
	axis2_options_set_property (_client_options.get(), env.get(), "KEY_FILE",    	 sslConf.ssl_client_key_prop.get());
	axis2_options_set_property (_client_options.get(), env.get(), "SSL_PASSPHRASE",   sslConf.passphrase_prop.get());
};

/**
 * Register HTTP Authentication
 *
 * @access protected
 * @return void
 */
void Axis2Client :: setHttpAuth()
{
	if(!httpAuth.http_username.empty() && !httpAuth.http_password.empty())
	{
	   if(axis2_options_set_http_auth_info(
			_client_options.get(),
			env.get(),
			httpAuth.http_username.c_str(),
			httpAuth.http_password.c_str(),
			httpAuth.http_auth_type.c_str()) == AXIS2_SUCCESS)
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
void Axis2Client :: setSoapVersion()
{
    // set soap version
	if(tansportOpts.use_soap)
	{
		axis2_options_set_soap_version(_client_options.get(), env.get(), [this]()->int { return tansportOpts.soap_version == 1.1 ? 1 : 2; }());

		log((boost::format("useSOAP TRUE setting soap version %1%") % tansportOpts.soap_version).str(), __FILE__,__LINE__);
	}
	//enable rest
	else
	{
		axis2_options_set_enable_rest(_client_options.get(), env.get(), AXIS2_TRUE);
		//*rest_enabled = AXIS2_TRUE;
	}
};

/**
 * Set HTTP Transport
 */
void Axis2Client :: setHttpTransport()
{
	/**
	 * default header type is POST,
	 * so only setting the HTTP_METHOD if GET
	 */
	if(!tansportOpts.http_method.empty())
	{
		axutil_property_t *http_method_property = axutil_property_create (env.get());

		axutil_property_set_value(
			http_method_property, env.get(), axutil_strdup (env.get(), [this]()->const char *{ return tansportOpts.http_method.c_str();}())
		);

		axis2_options_set_property (_client_options.get(), env.get(), AXIS2_HTTP_METHOD, http_method_property);

		log((boost::format("Setting HTTPMethod %1% property") % tansportOpts.http_method.c_str()).str(), __FILE__,__LINE__);
	}
};

/**
 * Register endpoint and soap action
 */
void Axis2Client :: setEndpointAndSoapAction()
{
	axis2_endpoint_ref_t *to_epr = nullptr;

	//We must have an endpoint
	if(_wsmessage->hasEndpoint())
	{
		//The endpoint can be a string
		if(_wsmessage->_endpoint->is_string())
		{
			to_epr = axis2_endpoint_ref_create (
				env.get(), _wsmessage->_endpoint->get<const char *>()
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
		throw std::invalid_argument("missing endpoint");

	axis2_options_set_to (_client_options.get(), env.get(), to_epr);

	//Transport URL
	if(!tansportOpts.transport_url.empty())
	{
	    std::vector<char> char_transport_url(tansportOpts.transport_url.data(), tansportOpts.transport_url.data() + tansportOpts.transport_url.size() + 1u);

		axutil_property_t * transport_url = axutil_property_create_with_args(
			env.get(),0,0,0, char_transport_url.data()
		);

		axis2_options_set_property(_client_options.get(), env.get(), AXIS2_TRANSPORT_URL, transport_url);

		log((boost::format("Setting WS Transport URL :: %1%") % tansportOpts.transport_url.c_str()).str(), __FILE__,__LINE__);
	}

	//if client_options
	if(_wsmessage->hasAction())
	{
	    string action = _wsmessage->_action->get<string>();

	    std::vector<char> char_action(action.data(), action.data() + action.size() + 1u);

		axutil_string_t *action_string = axutil_string_create (env.get(), char_action.data());
		axis2_options_set_soap_action (_client_options.get(), env.get(), action_string);

		log((boost::format("Setting WS SOAP Action :: %1%") % action.c_str()).str(), __FILE__,__LINE__);
	}
};

/**
 * Set WS-Addressing options
 */
void Axis2Client :: setWSAOpts() {

	log("Setting addressing options", __FILE__,__LINE__);

	if      (addressingOpts.use_wsa == "1.0")			{ log("useWSA true, version 1.0", __FILE__,__LINE__); }
	else if (addressingOpts.use_wsa == "submission") 	{ log("useWSA true, version is submission", __FILE__,__LINE__); }
	else 							  	{ log("useWSA false, disabled", __FILE__,__LINE__); addressingOpts.is_addressing_engaged = false; }

	if(!addressingOpts.use_wsa.empty())
	{
		if(_wsmessage->hasAction())
		{
			axis2_options_set_action (_client_options.get(), env.get(), _wsmessage->_action->get<const char *>());
			addressingOpts.is_addr_action_present = true;

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
				epr = axis2_endpoint_ref_create (env.get(), param->get<const char *>());

				log((boost::format("Addressing %1% is :- %2%") %type.c_str() %param->get<const char *>()).str(), __FILE__,__LINE__);
			}
			else if (param->is_array())
			{
				epr = add_endpoint_values_ref(param->get<map<string, vector<string>>>());

				log((boost::format("Addressing params array for param :: %1%") %type.c_str()).str(), __FILE__,__LINE__);
			}

			if(type == WSF_REPLY_TO) 		{ axis2_options_set_reply_to (_client_options.get(), env.get(), epr); }
			else if (type == WSF_FAULT_TO)  { axis2_options_set_fault_to (_client_options.get(), env.get(), epr); }
			else if( type == WSF_FROM) 		{ axis2_options_set_from     (_client_options.get(), env.get(), epr); }

			return true;
		};
		//----------------------------------
		//End Lambda

		set_addressing(WSF_FAULT_TO);
		set_addressing(WSF_REPLY_TO);
		set_addressing(WSF_FROM);

		if(_wsmessage->_wsa_must_understand)
		{
		    std::string str = "true";
		    std::vector<char> buffer_str(str.data(), str.data() + str.size() + 1u);

			axutil_property_t *must_understand_prop =
				axutil_property_create_with_args(
					env.get(), AXIS2_SCOPE_REQUEST, false, NULL, buffer_str.data()
				);

			if(must_understand_prop)
			{
				axis2_options_set_property(
					_client_options.get(), env.get(), "addMustUnderstandToAddr", must_understand_prop
				);
			}
		}
	}

	if(!addressingOpts.use_wsa.empty() && addressingOpts.is_addr_action_present == true)
	{
		axis2_svc_client_engage_module (_svc_client.get(), env.get(), WSF_MODULE_ADDRESSING);

		log("DEBUG ------> Engaged Addressing module", __FILE__,__LINE__);

		if(addressingOpts.use_wsa == "submission")
		{
			axutil_property_t *prop = axutil_property_create_with_args (env.get(), 0, AXIS2_TRUE,
				0, axutil_strdup (env.get(), AXIS2_WSA_NAMESPACE_SUBMISSION));

			axis2_options_set_property (_client_options.get(), env.get(), AXIS2_WSA_VERSION, prop);

			log("Addressing version is submission", __FILE__,__LINE__);
		}

		addressingOpts.is_addressing_engaged = true;
	}
};

/**
 * Timeout
 */
void Axis2Client :: setTimeout()
{
	axis2_options_set_timeout_in_milli_seconds(
		_client_options.get(), env.get(), 1000 * tansportOpts.timeout
	);
}

/**
 * Registering headers
 */
bool Axis2Client :: setSoapHeaders()
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

				om_str_out = axiom_node_to_string (header_node, env.get());

				AXIS2_LOG_DEBUG (env.get()->log, AXIS2_LOG_SI,
				WSF_PHP_LOG_PREFIX "HEADER_NODES ::::::::: \n\t %s \n", om_str_out);

				AXIS2_FREE(env.get()->allocator, om_str_out);
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
			env.get(),
			wsheader->_ns.c_str(),
			wsheader->_prefix.c_str()
		);
	}
	else if(wsheader->hasNamespace())
	{
		std::string prefix = "ns" + std::to_string(_ns_index);

		header_ns = axiom_namespace_create (
			env.get(),
			wsheader->_ns.c_str(),
			prefix.c_str()
		);
	}

	header_element = axiom_element_create (
		env.get(), parent , wsheader->_name.c_str(), header_ns, &header_node
	);

	if(wsheader->_must_understand)
	{
		log("must understand value is set", __FILE__,__LINE__);

		string must_understand = to_string(wsheader->_must_understand);

		soap_ns = axiom_namespace_create (env.get(), soap_uri, "soapenv");

		axiom_attribute_t *mu_attr = axiom_attribute_create (
			env.get(), AXIOM_SOAP_ATTR_MUST_UNDERSTAND, must_understand.c_str(), soap_ns
		);

		axiom_element_add_attribute (header_element, env.get(), mu_attr, header_node);
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
		soap_ns = axiom_namespace_create (env.get(), soap_uri, AXIOM_SOAP_DEFAULT_NAMESPACE_PREFIX);

	if (soap_version == AXIOM_SOAP12 )//&& roleval)
	{
		role_attr = axiom_attribute_create (env.get(), WSF_HEADER_ROLE, roleval.c_str(), soap_ns);
	    axiom_element_add_attribute (header_element, env.get(), role_attr, header_node);
	}
	else if (soap_version == AXIOM_SOAP11 )// && roleval)
	{
		role_attr = axiom_attribute_create (env.get(), WSF_HEADER_ACTOR, roleval.c_str(), soap_ns);
		axiom_element_add_attribute (header_element, env.get(), role_attr, header_node);
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
		std::unique_ptr<axiom_node_t, std::function<void(axiom_node_t *)>> node{nullptr,
		    [&](axiom_node_t * p)->void { if(p) std::free(p); }};
		try
		{
			stringstream xml; xml << wsheader->get_headers<string>();

			read_xml(xml, pt);
			node = getAxiomRootElementByXMLString(wsheader->get_headers<string>());

			if(node)
				axiom_node_add_child(header_node, env.get(), node.get());
		}
		catch (...)
		{
			axiom_element_set_text(
				header_element, env.get(), wsheader->get_headers<string>().c_str(), header_node
			);
		}
	}

	return header_node;
}

/**
 * Register security via WSSecToken and WSPolicy
 *
 * @access protected
 *
 * @return void
 */
void Axis2Client :: setWSSecurityOpts()
{
	std::unique_ptr<axiom_node_t, std::function<void(axiom_node_t *)>> policy_node{nullptr,
	     [&](axiom_node_t * p)->void { if(p) std::free(p); }
	};

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
		return ;

	if(_wspolicy)
	{
		if(_wspolicy->hasXMLPolicy())
		{
			log("POLICY :: XML found", __FILE__,__LINE__);
			policy_node = getAxiomRootElementByXMLString(_wspolicy->_policy);
		}
		else
		{
			policy_node = createPolicy();
		}
	}

	rampartOpts.rampart_ctx.reset(rampart_context_create(env.get()));

	setRampartOptions();

	if(rampart_context_get_custom_tokens(rampartOpts.rampart_ctx.get(), env.get()) && _wspolicy == NULL)
		return ;


	if(!_wspolicy &&  rampart_context_get_custom_tokens(rampartOpts.rampart_ctx.get(), env.get()) != NULL)
	{
		neethi_options_t *neethi_options = neethi_options_create (env.get());
		policy_node.reset(neethi_options_get_root_node (neethi_options, env.get()));
	}

	if(policy_node)
	{
		if(axiom_node_get_node_type (policy_node.get(), env.get()) == AXIOM_ELEMENT)
		{
			root_ele = (axiom_element_t *)axiom_node_get_data_element (policy_node.get(), env.get());

			if (root_ele)
			{
				neethi_policy = neethi_engine_get_policy (env.get(), policy_node.get(), root_ele);
				if (!neethi_policy)
				{
					return ;
				}
				svc = axis2_svc_client_get_svc (_svc_client.get(), env.get());
				if (!svc)
				{
					return ;
				}
				desc = axis2_svc_get_base (svc, env.get());
				if (!desc)
				{
					return ;
				}
				policy_include = axis2_desc_get_policy_include (desc, env.get());
				if (!policy_include)
				{
					return ;
				}
				axis2_policy_include_add_policy_element (policy_include, env.get(), AXIS2_SERVICE_POLICY, neethi_policy);

				svc_ctx  		= axis2_svc_client_get_svc_ctx (_svc_client.get(), env.get());
				conf_ctx 	    = axis2_svc_ctx_get_conf_ctx   (svc_ctx, env.get());
				conf 	 		= axis2_conf_ctx_get_conf      (conf_ctx, env.get());

				//Copy to buffer for C compat
				std::string param_name          = "RampartConfiguration";
				std::vector<char> buffer_param(param_name.data(), param_name.data() + param_name.size() + 1u);
				security_param                  = axutil_param_create(env.get(), buffer_param.data(), rampartOpts.rampart_ctx.get());

				axis2_conf_add_param (conf, env.get(), security_param);
			}
		}
	}
	axis2_svc_client_engage_module (_svc_client.get(), env.get(), WSF_MODULE_SECURITY);

	//DEBUG = 4
	if (policy_node)
	{
		axis2_char_t *om_str_out = nullptr;

		om_str_out = axiom_node_to_string (policy_node.get(), env.get());

		AXIS2_LOG_DEBUG (env.get()->log, AXIS2_LOG_SI,
		WSF_PHP_LOG_PREFIX "creating rampart client outgoing policy node \n\t %s \n", om_str_out);

		AXIS2_FREE(env.get()->allocator, om_str_out);
		om_str_out = NULL;
	}

	return ;
};

/**
 * Set endpoint and WSA params
 *
 * @access private
 * @return axis2_endpoint_ref_t *
 */
axis2_endpoint_ref_t * Axis2Client :: add_endpoint_values_ref(map<string, vector<string>> param)
{
	axis2_endpoint_ref_t *endpoint_ref = nullptr;
	string address 			  		   = param[WSF_WSA_ADDRESS][0];

	//Lambda to add a reference parameter in the form of an AXIOM node
	//for both metas and params
	auto set_wsa_params = [this, &endpoint_ref] ( vector<string> vector, const string &type ) mutable
	{
		if(vector.size() > 0)
		{
			for(auto &xml_str: vector)
			{
			    std::unique_ptr<axiom_node_t, std::function<void(axiom_node_t *)>> node{nullptr,
			        [&](axiom_node_t * p)->void {if(p) std::free(p);
			    }};
				node = getAxiomRootElementByXMLString(xml_str);

				axis2_endpoint_ref_add_ref_param(endpoint_ref, env.get(), node.get());

				log((boost::format("Setting WS-Adressing %1% :: %2%")
					% type.c_str()
					% xml_str.c_str()
				).str(), __FILE__,__LINE__);
			}
		}
	};
	//-- END LAMDA --

	if(address.empty())
	    throw std::invalid_argument("no suitable endpoint in WSMessage - messaging");

	endpoint_ref = axis2_endpoint_ref_create(env.get(), address.c_str());

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
 * @return void
 *
 */
void Axis2Client :: setRampartOptions()
{
    //Do we have a rampart context
	if(!rampartOpts.rampart_ctx.get())
	{
		log("RAMPART :: rampart context null -- abort security", __FILE__,__LINE__);

		return ;
	}

	//affect or return false;
	if(_wssectoken)
	{
	    log("RAMPART :: Setting values to rampart context ", __FILE__,__LINE__);

        rampart_context_set_ttl(rampartOpts.rampart_ctx.get(), env.get(), _wssectoken->_ttl);

        //clockSkewBuffer @undocumented from api
        //rampart_context_set_clock_skew_buffer(rampart_context, env, Z_LVAL_PP(token_val));

        /*
         * Set the username
         */
        if(_wssectoken->hasUser())
        {
            rampartOpts.username = std::vector<char>(_wssectoken->_user.data(),
                    _wssectoken->_user.data() + _wssectoken->_user.size() + 1u);

            rampart_context_set_user(rampartOpts.rampart_ctx.get(), env.get(), rampartOpts.username.data());

            log("RAMPART :: Setting username", __FILE__,__LINE__);
        }

        /*
         * Set the certificate
         */
        if(_wssectoken->hasCertificate())
        {
            rampartOpts.certificate =  std::vector<char>(_wssectoken->_certificate.data(),
                    _wssectoken->_certificate.data() + _wssectoken->_certificate.size() + 1u);

            rampart_context_set_certificate(rampartOpts.rampart_ctx.get(), env.get(), rampartOpts.certificate.data());

            if(rampart_context_set_certificate_type(rampartOpts.rampart_ctx.get(), env.get(), AXIS2_KEY_TYPE_PEM) == AXIS2_SUCCESS)
            {
                log("RAMPART :: Setting certificate type", __FILE__,__LINE__);
            }
        }

        /*
         * Set the has receiver certificate
         */
        if(_wssectoken->hasReceiverCertificate())
        {
            rampartOpts.receiver_certificate = std::vector<char>(_wssectoken->_receiver_certificate.data(),
                    _wssectoken->_receiver_certificate.data() + _wssectoken->_receiver_certificate.size() + 1u);

            rampart_context_set_receiver_certificate(rampartOpts.rampart_ctx.get(), env.get(), rampartOpts.receiver_certificate.data());

            if(rampart_context_set_receiver_certificate_type(rampartOpts.rampart_ctx.get(), env.get(), AXIS2_KEY_TYPE_PEM) == AXIS2_SUCCESS)
            {
                log("RAMPART :: Setting receiver certificate type", __FILE__,__LINE__);
            }
        }

        /*
         * Set the private key
         */
        if(_wssectoken->hasPrivateKey())
        {
            rampartOpts.private_key =  std::vector<char>(_wssectoken->_private_key.data(),
                    _wssectoken->_private_key.data() + _wssectoken->_private_key.size() + 1u);

            rampart_context_set_prv_key(rampartOpts.rampart_ctx.get(), env.get(), rampartOpts.private_key.data());

            log("RAMPART :: Setting pvt key", __FILE__,__LINE__);

            if(rampart_context_set_prv_key_type (rampartOpts.rampart_ctx.get(), env.get(), AXIS2_KEY_TYPE_PEM) == AXIS2_SUCCESS)
            {
                log("RAMPART :: Setting pvt key format", __FILE__,__LINE__);
            }
        }

        /*
         * Set the password
         */
        if(_wssectoken->hasPassword())
        {
            rampartOpts.password = std::vector<char>(_wssectoken->_password.data(),
                    _wssectoken->_password.data() + _wssectoken->_password.size() + 1u);

            rampart_context_set_password(rampartOpts.rampart_ctx.get(), env.get(), rampartOpts.password.data());

            log("RAMPART :: Setting password " + _wssectoken->_password, __FILE__,__LINE__);
        }

        //TODO: Implement Password Callback      @wsf_policy.c:622
        //TODO: Implement PCKS12 KeyStore        @wsf_policy.c:638
        //TODO: Implement Replay Detect Callback @wsf_policy.c:643
        //TODO: Implement Enable RDetect Callb   @wsf_policy.c:658
        //TODO: Implement Custom Token           @wsf_policy.c:664

        log("RAMPART :: Rampart Ready", __FILE__,__LINE__);
	}
}

/**
 * Create Policy node from WSPolicy Object
 *
 * @access private
 * @return axiom_node_t * - A node reprensenting the policy
 */
std::unique_ptr<axiom_node_t, std::function<void(axiom_node_t *)>> Axis2Client :: createPolicy()
{
	neethi_options_t 	*neethi_options = nullptr;

	std::unique_ptr<axiom_node_t, std::function<void(axiom_node_t *)>> return_node{nullptr,
	    [&](axiom_node_t * p)->void { if(p) std::free(p); }
	};

	if(!_wspolicy) return nullptr;

	log("NEETHI :: Creating the policy node", __FILE__,__LINE__);

	neethi_options = neethi_options_create (env.get());

	if (neethi_options)
	{
		//Has Timestamp
		if(_wspolicy->hasTimestamp())
		{
			if (neethi_options_set_include_timestamp (neethi_options, env.get(), AXIS2_TRUE))
				log("NEETHI :: Timestamp_enabled", __FILE__,__LINE__);
		}

		//Has Username Token
		if(_wspolicy->hasUsernameToken())
		{
			if (neethi_options_set_is_username_token (neethi_options, env.get(), AXIS2_TRUE))
				log("NEETHI :: Token reference_enabled", __FILE__,__LINE__);
		}

		//Has Policy encrypted
		if(_wspolicy->hasEncrypted())
		{
			if (neethi_options_set_encrypt_body (neethi_options, env.get(), AXIS2_TRUE))
				log("NEETHI :: Encrypt body option enabled", __FILE__,__LINE__);
		}

		if(_wspolicy->hasAlgoSuite())
		{
		    std::vector<char> buffer_algosuite(_wspolicy->_algorithm_suite.data(),
		            _wspolicy->_algorithm_suite.data() + _wspolicy->_algorithm_suite.size() + 1u);

			if (neethi_options_set_algorithmsuite (neethi_options, env.get(), buffer_algosuite.data()))
				log("NEETHI :: AlgorithmSuite enabled", __FILE__,__LINE__);
		}

		if(_wspolicy->hasSigned())
		{
			if (neethi_options_set_sign_body (neethi_options, env.get(), AXIS2_TRUE))
				log("NEETHI :: Sign_enabled", __FILE__,__LINE__);
		}

		if(_wspolicy->hasSecurityTokenRef())
		{
		    std::vector<char> buffer_tokenValue(_wspolicy->getRampartTokenValue().data(),
		            _wspolicy->getRampartTokenValue().data() + _wspolicy->getRampartTokenValue().size() + 1u);

			if (neethi_options_set_keyidentifier (neethi_options, env.get(), buffer_tokenValue.data()))
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
				if (neethi_options_set_encrypt_before_sign (neethi_options, env.get(), AXIS2_TRUE))
					log("NEETHI :: Encrypt_before_sign_enabled", __FILE__,__LINE__);

			}
			else if(_wspolicy->_protection_order == "SignBeforeEncrypt")
			{
				if (neethi_options_set_encrypt_before_sign (neethi_options, env.get(), AXIS2_FALSE))
					log("Sign_before_encrption_enabled", __FILE__,__LINE__);
			}
			else
			{
				log("NEETHI :: Wrong Protection Order", __FILE__,__LINE__);
			}
		}

		if(_wspolicy->hasSecurityBinding())
		{
		    std::string binding = _wspolicy->_protection_order == "Asymmetric" ? "AsymmetricBinding" : "SymmetricBinding";
		    std::vector<char> buffer_binding(_wspolicy->_protection_order.data(),
		            _wspolicy->_protection_order.data() + _wspolicy->_protection_order.size() + 1u);

		    neethi_options_set_binding(neethi_options, env.get(), buffer_binding.data());
		}
	}

	if (neethi_options)
	{
		return_node.reset(neethi_options_get_root_node (neethi_options, env.get()));
	}

	return return_node;
}

/**
 * Register reliable messaging
 *
 *
 */
void Axis2Client :: setWSReliableOpts()
{
	axutil_property_t *rm_prop 	= nullptr;

	if(reliableOpts.reliable_version > 0)
	{
	    auto spec_policy = [this]()->std::string{
	        return reliableOpts.reliable_version == 1 ? "Spec_2005_02" : "Spec_2007_02";
	    }();

	    std::vector<char> buffer_spec_policy(spec_policy.data(),
	            spec_policy.data() + spec_policy.size() + 1u);

		rm_prop = axutil_property_create_with_args (env.get(), 0, 0, 0, buffer_spec_policy.data());

		axis2_options_set_property (_client_options.get(), env.get(), WSF_SANDESHA2_CLIENT_RM_SPEC_VERSION, rm_prop);

		log("RM version > 0", __FILE__,__LINE__);

		//RM must be engaged
		reliableOpts.engage_rm = true;
	}

	//
	if(hasReliable())
	{
		if(!addressingOpts.is_addressing_engaged)
		{
			//set addressing
			log("DEBUG-------> is_addressing_engaged false - TO BE Implemented", __FILE__,__LINE__);
		}
		axis2_svc_client_engage_module (_svc_client.get(), env.get(), WSF_MODULE_ADDRESSING);

		log("useWSA not specified, addressing engaged since rm is engaged", __FILE__,__LINE__);

		axis2_svc_client_engage_module (_svc_client.get(), env.get(), WSF_MODULE_RM);

		reliableOpts.is_engaged_rm = true;

		//set_module_option("sandesha2", "InactivityTimeout", to_string(_sequence_expiry_time));

		//log("sequenceExpiryTime setted");

		//axutil_property_t *seq_property = NULL;
		//sequence_key = axutil_strdup (env.get(), get_axis2_char_ptr(_sequence_key));

		//Two way single channel
		char *offered_seq_id = NULL;

		axutil_property_t *sequence_property = NULL;
		offered_seq_id 						 = axutil_uuid_gen (env.get());
		sequence_property 					 = axutil_property_create (env.get());

		axutil_property_set_value (sequence_property, env.get(), axutil_strdup (env.get(), offered_seq_id));
		axis2_options_set_property (_client_options.get(), env.get(), "Sandesha2OfferedSequenceId" , sequence_property);

		log ("Sandesha2OfferedSequenceId is set as property", __FILE__,__LINE__);

		//Test if we defined a TIMEOUT for RM
		/** default timeout value is 5 */
		char * timeout 						= WSF_RM_DEFAULT_RESPONSE_TIMEOUT;
		axutil_property_t *timeout_property = axutil_property_create_with_args (env.get(), 0, 0, 0, timeout);

		if (timeout_property)
		{
			axis2_options_set_property (_client_options.get(), env.get(), "time_out", timeout_property);
		}
	}
	//

	//RM is now engaged

	//IF Seq expiry time / InactivityTimeout
	//set_module_option("sandesha2", "InactivityTimeout", "60");

	//log("sequenceExpiryTime is 50");

	//IF sequenceKey
	//axutil_property_t *seq_property = nullptr;
	//sequence_key 					= axutil_strdup (env.get(), Z_STRVAL_PP (client_tmp));

	if(reliableOpts.reliable_version == 1)
	{
	    std::string str = "true";
	    std::vector<char> buffer_str(str.data(),
	            str.data() + str.size() + 1u);

		axutil_property_t *last_msg_prop = axutil_property_create_with_args (
			 env.get(), 0, 0, 0, buffer_str.data()
		);

		 axis2_options_set_property (
			_client_options.get(), env.get(), "Sandesha2LastMessage", last_msg_prop
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
bool Axis2Client :: hasReliable()
{
	return ((addressingOpts.is_addressing_engaged ||
			(!addressingOpts.is_addressing_engaged && addressingOpts.is_addr_action_present)) && reliableOpts.engage_rm ) ? true : false;
};

/**
 * Has Fault on the last query
 *
 *
 */
bool Axis2Client :: hasSoapFault()
{
	 return axis2_svc_client_get_last_response_has_fault(_svc_client.get(), env.get());
};

/**
 * Get Last Response as String format
 *
 * @access protected
 * @return string
 */
string Axis2Client :: getSoapResponse()
{
    //std::unique<axiom_xml_writer_t, >

	axiom_xml_writer_t * writer 	    = nullptr;
	axiom_output_t     * om_output 		= nullptr;
	axis2_char_t       * buffer 		= nullptr;

	/*
	 * @TODO: Please backport attachements @see MTOM
	 */
	//axiom_soap_envelope_t *soapenvelope = nullptr;
	//soapenvelope = axis2_svc_client_get_last_response_soap_envelope (_svc_client.get(), env.get());
	//attachments_found = wsf_client_handle_incoming_attachments (env, client_ht, rmsg,
	//                soap_envelope, response_payload TSRMLS_CC);
	writer = axiom_xml_writer_create_for_memory (env.get(), nullptr, AXIS2_TRUE, 0,
			AXIS2_XML_PARSER_TYPE_BUFFER);

	om_output = axiom_output_create (env.get(), writer);

	axiom_node_serialize_sub_tree(response.payload.get(), env.get(), om_output);

	buffer = (axis2_char_t *) axiom_xml_writer_get_xml (writer, env.get());

	string response(buffer);

	axiom_output_free(om_output, env.get());

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
		detail_entry_node = axiom_node_get_first_element(detail_node, env.get());
		if(detail_entry_node)
		{
			detail_string = axiom_node_to_string(detail_entry_node, env.get());

			if(detail_string) { env.get()->allocator->free_fn(env.get()->allocator, detail_string); }
		}
	}

	if(!detail_string) {
	    detail_string = "n/a";
	}

	return detail_string;
}

/**
 *
 */
string Axis2Client::get_soap_fault_reason(axiom_node_t *reason_node, axiom_element_t *reason_element)
{
	axis2_char_t *text_value = NULL;

	if([this]()->int { return tansportOpts.soap_version == 1.1 ? 1 : 2; }() == AXIOM_SOAP12)
	{
		axiom_node_t 	* text_node 	= nullptr;
		axiom_element_t * text_element 	= nullptr;

		text_element = axiom_element_get_first_element(reason_element, env.get(), reason_node, &text_node);
		if(text_element)
		{
			text_value = axiom_element_get_text(text_element, env.get(), text_node);
		}
	}
	else if([this]()->int { return tansportOpts.soap_version == 1.1 ? 1 : 2; }() == AXIOM_SOAP11)
	{
		text_value = axiom_element_get_text(reason_element, env.get(), reason_node);
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

	if([this]()->int { return tansportOpts.soap_version == 1.1 ? 1 : 2; }() == AXIOM_SOAP12)
	{
		code_value_ele = axiom_element_get_first_element(code_element, env.get(), code_node, &code_value_node);
		code 		   = axiom_element_get_text(code_value_ele, env.get(), code_value_node);
	}
	else if([this]()->int { return tansportOpts.soap_version == 1.1 ? 1 : 2; }() == AXIOM_SOAP11)
	{
		code 		   = axiom_element_get_text(code_element, env.get(), code_node);
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

		fault_node = axiom_soap_fault_get_base_node(soap_fault, env.get());

		if(fault_node)
		{
			fault.node = (string)axiom_node_to_string (fault_node, env.get());

			axiom_element_t  				* fault_element = nullptr;
			axiom_child_element_iterator_t  * ele_iterator 	= nullptr;

			fault_element = (axiom_element_t *)axiom_node_get_data_element(fault_node, env.get());
			ele_iterator  = axiom_element_get_child_elements(fault_element, env.get(), fault_node);

			if(ele_iterator)
			{
				axiom_node_t     * node    = nullptr;
				axiom_element_t  * element = nullptr;

				while(axiom_child_element_iterator_has_next(ele_iterator, env.get()))
				{
					node = axiom_child_element_iterator_next(ele_iterator, env.get());
					if(node)
					{
						element = (axiom_element_t *) axiom_node_get_data_element(node, env.get());

						if(element)
						{
							char * localname = nullptr;
							localname        = axiom_element_get_localname(element, env.get());

							if(localname)
							{
								if      (std::strcmp(localname, AXIOM_SOAP12_SOAP_FAULT_CODE_LOCAL_NAME)   == 0) { fault.code   = get_soap_fault_code(node, element);   }
								else if (std::strcmp(localname, AXIOM_SOAP11_SOAP_FAULT_CODE_LOCAL_NAME)   == 0) { fault.code   = get_soap_fault_code(node, element);   }
								else if (std::strcmp(localname, AXIOM_SOAP12_SOAP_FAULT_REASON_LOCAL_NAME) == 0) { fault.reason = get_soap_fault_reason(node, element); }
								else if (std::strcmp(localname, AXIOM_SOAP11_SOAP_FAULT_STRING_LOCAL_NAME) == 0) { fault.reason = get_soap_fault_reason(node, element); }
								else if (std::strcmp(localname, AXIOM_SOAP12_SOAP_FAULT_DETAIL_LOCAL_NAME) == 0 ||
										 std::strcmp(localname, AXIOM_SOAP11_SOAP_FAULT_DETAIL_LOCAL_NAME)) 	 { fault.details = get_soap_fault_detail(node, element); }
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
Axis2Client::FaultType Axis2Client :: getSoapFault()
{
	FaultType                 fault;
	axiom_soap_envelope_t   * soap_envelope   = nullptr;
	axiom_soap_body_t       * soap_body       = nullptr;
	axiom_soap_fault_t      * soap_fault 	  = nullptr;
	axis2_char_t 	        * res_text 	      = nullptr;

	soap_envelope = axis2_svc_client_get_last_response_soap_envelope (_svc_client.get(), env.get());

	if(soap_envelope)
	{
		soap_body  = axiom_soap_envelope_get_body (soap_envelope, env.get());

		if(soap_body)
		{
			soap_fault = axiom_soap_body_get_fault (soap_body, env.get());

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
