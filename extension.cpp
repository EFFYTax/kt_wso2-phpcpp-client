#include <phpcpp.h>
#include <iostream>
#include "include/ws_message.h"
#include "include/ws_client.h"
#include "include/ws_client_proxy.h"
#include "include/ws_data.h"
#include "include/ws_fault.h"
#include "include/ws_header.h"
#include "include/ws_policy.h"
#include "include/ws_security_token.h"
#include "include/ws_utils.h"
#include "include/ws_version.h"

/**
 *  tell the compiler that the get_module is a pure C function
 */
extern "C" {
    
    PHPCPP_EXPORT void *get_module() 
    {
        // static(!) Php::Extension object that should stay in memory
        // for the entire duration of the process (that's why it's static)
        static Php::Extension ktwso2("ktwso2", "0.0.1");
        
        // description of the class so that PHP knows which methods are accessible
        Php::Class<WSClient>  	    wsclient("WSClient");
        Php::Class<WSMessage> 	    wsmessage("WSMessage");
        Php::Class<WSClientProxy>   wsclientproxy("WSClientProxy");
        Php::Class<WSData> 		    wsdata("WSData");
        Php::Class<WSFault> 	    wsfault("WSFault");
        Php::Class<WSHeader> 	    wsheader("WSHeader");
        Php::Class<WSPolicy> 	    wspolicy("WSPolicy");
        Php::Class<WSUtils> 		wsutils("WSUtils");
        Php::Class<WSSecurityToken> wssecuritytoken("WSSecurityToken");

        // WSClient - Registering methods
        wsclient.method("__construct", &WSClient::__construct);

        wsclient.method("request", &WSClient::request);

        wsclient.method("setMessage",  &WSClient::set_wsmessage);
        wsclient.method("setSecToken", &WSClient::set_wssectoken);
        wsclient.method("setPolicy",   &WSClient::set_wspolicy);

        wsclient.method("getMessage",  &WSClient::get_wsmessage);
        wsclient.method("getSecToken", &WSClient::get_wssectoken);
        wsclient.method("getDebug",    &WSClient::get_debug);
        wsclient.method("disableSoap", &WSClient::disable_soap);

        //Use WSA
        wsclient.method("setWSA",           &WSClient::set_use_wsa);

        //Use SSL
        wsclient.method("setSSLClientCert", &WSClient::set_client_cert);
        wsclient.method("setSSLServerCert", &WSClient::set_server_cert);
        wsclient.method("setSSLPassphrase", &WSClient::set_ssl_passphrase);

        //Use HTTP Auth
        wsclient.method("setHTTPUsername", &WSClient::set_http_auth_username);
        wsclient.method("setHTTPPassword", &WSClient::set_http_auth_password);
        wsclient.method("setHTTPAuth",     &WSClient::set_http_auth_type);

        //Reliable
        wsclient.method("setReliable",      &WSClient::set_ws_reliable);
        wsclient.method("setSoapVersion",   &WSClient::set_soap_version);

        //Timeout
        wsclient.method("setTimeout",       &WSClient::set_timeout);


        /**
         * Setter for WSClient
         */
        /*
        wsclient.method("setUseSoap",      &WSClient::set_use_soap);
        wsclient.method("setHTTPMethod",   &WSClient::set_http_method);
        wsclient.method("setEndpoint",     &WSClient::set_endpoint);
        wsclient.method("setTransportURL", &WSClient::set_transport_url);
        wsclient.method("setWsAction",     &WSClient::set_ws_action);
        wsclient.method("setReplyTo",      &WSClient::set_reply_to);
        wsclient.method("setFaultTo",      &WSClient::set_fault_to);
        wsclient.method("setWSAdress",     &WSClient::set_wsa_address);
        wsclient.method("setWSParams",     &WSClient::set_wsa_ref_params);
        wsclient.method("setFrom",         &WSClient::set_from);
        wsclient.method("setWSAMustUnderstand", &WSClient::set_wsa_mustunderstand);

        wsclient.method("setProxyPort",    &WSClient::set_proxy_port);*/


        // WSMessage - Registering methods
        wsmessage.method("__construct", &WSMessage::__construct, {
        //	Php::ByVal("payload",    Php::Type::String, true),
		//	Php::ByVal("properties", Php::Type::Array,  false),
        });

       wsmessage.method("setPayload", 				&WSMessage::set_payload);
       wsmessage.method("setEndpoint",  			&WSMessage::set_endpoint);
       wsmessage.method("setAction",  				&WSMessage::set_action);
       wsmessage.method("setFrom",     				&WSMessage::set_from);
       wsmessage.method("setReply",     			&WSMessage::set_reply_to);
       wsmessage.method("setFault",     	  		&WSMessage::set_fault);
       wsmessage.method("setMustUnderstand", 		&WSMessage::set_must_understand);
       wsmessage.method("setAttachments",       	&WSMessage::set_attachments);
       wsmessage.method("setAttachmentContentType",	&WSMessage::set_attachment_content_type);
       wsmessage.method("setHeaders",				&WSMessage::set_headers);
       wsmessage.method("setRestContentType",       &WSMessage::set_rest_content_type);
       wsmessage.method("getHeaders",               &WSMessage::get_headers<Php::Value>);

       wsmessage.method("getDebug",       &WSMessage::get_debug);
       wsmessage.method("getResponse",    &WSMessage::getResponse);
       //wsmessage.method("getPayload",  &WSMessage::get_payload);
        //wsmessage.method("getTo",       &WSMessage::get_to);

        //wsmessage.property("str", &WSMessage::getStr, &WSMessage::setStr);
        //wsmessage.property("str", "", Php::Public);

        //wsmessage.property(WSF_ATTACHMENTS, Php::Public);


        // WSClientProxy - Registering methods
        wsclientproxy.method("__construct", &WSClientProxy::__construct);

        // WSData - Registering methods
        wsdata.method("__construct", &WSData::__construct);

        // WSFault - Registering methods
        wsfault.method("__construct", &WSFault::__construct);

        // WSHeader - Registering methods
        wsheader.method("__construct", 		 &WSHeader::__construct);
        wsheader.method("setNs",       		 &WSHeader::set_ns);
        wsheader.method("setPrefix",  	 	 &WSHeader::set_prefix);
        wsheader.method("setName",     		 &WSHeader::set_name);
        wsheader.method("setData",     		 &WSHeader::set_data);
        wsheader.method("setRole",    	     &WSHeader::set_role);

        wsheader.method("getNs",             &WSHeader::get_ns);
        wsheader.method("getPrefix",         &WSHeader::get_prefix);
        wsheader.method("getName",           &WSHeader::get_name);
        wsheader.method("getData",           &WSHeader::get_data);
        wsheader.method("getRole",           &WSHeader::get_role);

        wsheader.method("isMustUnderstand",  &WSHeader::is_must_understand);
        wsheader.method("setMustUnderstand", &WSHeader::set_must_understand);

        //wsheader.property("ns", "", Php::Public);

        // WSPolicy - Registering methods
        wspolicy.method("__construct",    &WSPolicy::__construct);
        wspolicy.method("setXMLPolicy",   &WSPolicy::set_policy);

        // WSUtils - Registering methods
        wsutils.method("__construct", &WSUtils::__construct);

        // WSSecurityToken - Registering methods
        wssecuritytoken.method("__construct", 						&WSSecurityToken::__construct);
        wssecuritytoken.method("setUser", 							&WSSecurityToken::set_user);
        wssecuritytoken.method("setCertificate", 					&WSSecurityToken::set_certificate);
        wssecuritytoken.method("setPasswordType", 					&WSSecurityToken::set_password_type);
        wssecuritytoken.method("setPassword",                       &WSSecurityToken::set_password);
        wssecuritytoken.method("setPrivateKey",                     &WSSecurityToken::set_private_key);
        wssecuritytoken.method("setReceiverCertificate",            &WSSecurityToken::set_receiver_certificate);
        wssecuritytoken.method("setTTL", 							&WSSecurityToken::set_ttl);
        wssecuritytoken.method("setPasswordCallback", 				&WSSecurityToken::set_password_callback);
        wssecuritytoken.method("setReplayDetectionCallback",        &WSSecurityToken::set_replay_detection_callback);
        wssecuritytoken.method("setReplayDetectionCallbackData",    &WSSecurityToken::set_replay_detection_callback_data);
        wssecuritytoken.method("setSCTStoreCallback", 				&WSSecurityToken::set_sct_store_callback);
        wssecuritytoken.method("setSCTGetCallback", 				&WSSecurityToken::set_sct_get_callback);
        wssecuritytoken.method("setSCTDeleteCallback", 				&WSSecurityToken::set_sct_delete_callback);
        wssecuritytoken.method("setSCTCallbackData",				&WSSecurityToken::set_sct_callback_data);


        ktwso2.add(std::move(wsclient));
        ktwso2.add(std::move(wsmessage));
        ktwso2.add(std::move(wsclientproxy));
        ktwso2.add(std::move(wsdata));
        ktwso2.add(std::move(wsfault));
        ktwso2.add(std::move(wsheader));
        ktwso2.add(std::move(wspolicy));
        ktwso2.add(std::move(wsutils));
        ktwso2.add(std::move(wssecuritytoken));

        ktwso2.add(Php::Ini("ktwso2.axis2", "default-value"));

        // return the extension
        return ktwso2;
    }
}
