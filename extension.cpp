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
        static Php::Extension extension("ktwso2", "0.0.1");
        
        //Put all objects inside a PHP Namespace
        Php::Namespace ns("KTWS");

        // description of the class so that PHP knows which methods are accessible
        Php::Class<WSClient>  	    wsclient        ("WSClient");
        Php::Class<WSMessage> 	    wsmessage       ("WSMessage");
        Php::Class<WSClientProxy>   wsclientproxy   ("WSClientProxy");
        Php::Class<WSData> 		    wsdata          ("WSData");
        Php::Class<WSFault> 	    wsfault         ("WSFault");
        Php::Class<WSHeader> 	    wsheader        ("WSHeader");
        Php::Class<WSPolicy> 	    wspolicy        ("WSPolicy");
        Php::Class<WSUtils> 		wsutils         ("WSUtils");
        Php::Class<WSSecurityToken> wssecuritytoken ("WSSecurityToken");

        // WSClient - Registering methods
        wsclient.method<&WSClient::__construct>             ("__construct");
        wsclient.method<&WSClient::request> 	            ("request");
        wsclient.method<&WSClient::set_wsmessage>           ("setMessage");
        wsclient.method<&WSClient::set_wssectoken>          ("setSecToken");
        wsclient.method<&WSClient::set_wspolicy>            ("setPolicy");
        wsclient.method<&WSClient::get_wsmessage>           ("getMessage");
        wsclient.method<&WSClient::get_wssectoken>          ("getSecToken");


        //Work w/ REST
        wsclient.method<&WSClient::disable_soap>            ("disableSoap");

        //Use WSA
        wsclient.method<&WSClient::set_use_wsa>             ("setWSA");
        wsclient.method<&WSClient::set_wsa_address>         ("setWSAddress");
        wsclient.method<&WSClient::set_wsa_ref_params>      ("setWSARefParams");

        //Proxy
        wsclient.method<&WSClient::set_proxy_auth_username>  ("setProxyUsername");
        wsclient.method<&WSClient::set_proxy_auth_password>  ("setProxyPassword");
        wsclient.method<&WSClient::set_proxy_auth_type>      ("setProxyAuthType");
        wsclient.method<&WSClient::set_proxy_host>           ("setProxyHost");
        wsclient.method<&WSClient::set_proxy_port>           ("setProxyPort");

        //Use SSL
        wsclient.method<&WSClient::set_client_cert>         ("setSSLClientCert");
        wsclient.method<&WSClient::set_server_cert>         ("setSSLServerCert");
        wsclient.method<&WSClient::set_ssl_passphrase>      ("setSSLPassphrase");

        //Use HTTP Auth
        wsclient.method<&WSClient::set_http_auth_username>  ("setHTTPUsername");
        wsclient.method<&WSClient::set_http_auth_password>  ("setHTTPPassword");
        wsclient.method<&WSClient::set_http_auth_type>      ("setHTTPAuth");
        wsclient.method<&WSClient::set_http_method>         ("setHTTPMethod");

        //Transport URL
        wsclient.method<&WSClient::set_transport_url>       ("setTransportURL");

        //Reliable
        wsclient.method<&WSClient::set_ws_reliable> 	    ("setReliable");
        wsclient.method<&WSClient::set_soap_version>        ("setSoapVersion");

        //Timeout
        wsclient.method<&WSClient::set_timeout>             ("setTimeout");

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
       wsmessage.method<&WSMessage::__construct> ("__construct");

       wsmessage.method<&WSMessage::set_payload>  					("setPayload");
       wsmessage.method<&WSMessage::set_endpoint> 					("setEndpoint");
       wsmessage.method<&WSMessage::set_action>   					("setAction");
       wsmessage.method<&WSMessage::set_from>     					("setFrom");
       wsmessage.method<&WSMessage::set_reply_to> 					("setReply");
       wsmessage.method<&WSMessage::set_fault>    					("setFault");
       wsmessage.method<&WSMessage::set_must_understand> 			("setMustUnderstand");
       wsmessage.method<&WSMessage::set_attachments> 				("setAttachments");
       wsmessage.method<&WSMessage::set_attachment_content_type>	("setAttachmentContentType");
       wsmessage.method<&WSMessage::set_headers>   					("setHeaders");
       wsmessage.method<&WSMessage::set_rest_content_type>          ("setRestContentType");
       wsmessage.method<&WSMessage::get_headers<Php::Value>>		("getHeaders");

       wsmessage.method<&WSMessage::get_payload>                    ("getPayload");
       wsmessage.method<&WSMessage::get_from>                       ("getFrom");
       wsmessage.method<&WSMessage::get_endpoint>                   ("getEndpoint");
       wsmessage.method<&WSMessage::get_action>                     ("getAction");
       wsmessage.method<&WSMessage::get_reply_to>                   ("getReply");
       wsmessage.method<&WSMessage::get_fault>                      ("getFault");
       wsmessage.method<&WSMessage::is_must_understand>             ("hasMustUnderstand");
       wsmessage.method<&WSMessage::get_rest_content_type>          ("getRestContentType");
       wsmessage.method<&WSMessage::get_debug>                      ("getDebug");
       wsmessage.method<&WSMessage::getResponse>                    ("getResponse");


       //wsmessage.method("getPayload",  &WSMessage::get_payload);
       //wsmessage.method("getTo",       &WSMessage::get_to);

       //wsmessage.property("str", &WSMessage::getStr, &WSMessage::setStr);
       //wsmessage.property("str", "", Php::Public);

       //wsmessage.property(WSF_ATTACHMENTS, Php::Public);


       // WSClientProxy - Registering methods
       wsclientproxy.method<&WSClientProxy::__construct> ("__construct");

       // WSData - Registering methods
       wsdata.method<&WSData::__construct> ("__construct");

       // WSFault - Registering methods
       wsfault.method<&WSFault::__construct> ("__construct");

       // WSHeader - Registering methods
       wsheader.method<&WSHeader::__construct> ("__construct");
       wsheader.method<&WSHeader::setNs>      ("setNs");
       wsheader.method<&WSHeader::setPrefix>  ("setPrefix");
       wsheader.method<&WSHeader::setName>    ("setName");
       wsheader.method<&WSHeader::setData>    ("setData");
       wsheader.method<&WSHeader::setRole>    ("setRole");

       wsheader.method<&WSHeader::getNs>      ("getNs");
       wsheader.method<&WSHeader::getPrefix>  ("getPrefix");
       wsheader.method<&WSHeader::getName>    ("getName");
       wsheader.method<&WSHeader::getData>    ("getData");
       wsheader.method<&WSHeader::getRole>    ("getRole");

       wsheader.method<&WSHeader::hasMustUnderstand>  ("hasMustUnderstand");
       wsheader.method<&WSHeader::setMustUnderstand>  ("setMustUnderstand");

       //wsheader.property("ns", "", Php::Public);

       // WSPolicy - Registering methods
       wspolicy.method<&WSPolicy::__construct> ("__construct");
       wspolicy.method<&WSPolicy::set_policy>  ("setXMLPolicy");

       // WSUtils - Registering methods
       wsutils.method<&WSUtils::__construct> ("__construct");

       // WSSecurityToken - Registering methods
       wssecuritytoken.method<&WSSecurityToken::__construct>   						("__construct");
       wssecuritytoken.method<&WSSecurityToken::set_user>    						("setUser");
       wssecuritytoken.method<&WSSecurityToken::set_certificate> 					("setCertificate");
       wssecuritytoken.method<&WSSecurityToken::set_password_type> 					("setPasswordType");
       wssecuritytoken.method<&WSSecurityToken::set_password>       				("setPassword");
       wssecuritytoken.method<&WSSecurityToken::set_private_key>    				("setPrivateKey");
       wssecuritytoken.method<&WSSecurityToken::set_receiver_certificate> 			("setReceiverCertificate");
       wssecuritytoken.method<&WSSecurityToken::set_ttl> 							("setTTL");
       wssecuritytoken.method<&WSSecurityToken::set_password_callback> 				("setPasswordCallback");
       wssecuritytoken.method<&WSSecurityToken::set_replay_detection_callback> 		("setReplayDetectionCallback");
       wssecuritytoken.method<&WSSecurityToken::set_replay_detection_callback_data> ("setReplayDetectionCallbackData");
       wssecuritytoken.method<&WSSecurityToken::set_sct_store_callback> 			("setSCTStoreCallback");
       wssecuritytoken.method<&WSSecurityToken::set_sct_get_callback> 				("setSCTGetCallback");
       wssecuritytoken.method<&WSSecurityToken::set_sct_delete_callback> 			("setSCTDeleteCallback");
       wssecuritytoken.method<&WSSecurityToken::set_sct_callback_data>		 		("setSCTCallbackData");

       ns.add(std::move(wsclient));
       ns.add(std::move(wsmessage));
       ns.add(std::move(wsclientproxy));
       ns.add(std::move(wsdata));
       ns.add(std::move(wsfault));
       ns.add(std::move(wsheader));
       ns.add(std::move(wspolicy));
       ns.add(std::move(wsutils));
       ns.add(std::move(wssecuritytoken));

       //Move extension to namespace
       extension.add(std::move(ns));

       //Add default ini configuration
       extension.add(Php::Ini("ktwso2.axis2_directory", ""));
       extension.add(Php::Ini("ktwso2.log_path",        ""));
       extension.add(Php::Ini("ktwso2.log_filename",    ""));
       extension.add(Php::Ini("ktwso2.log_level",        0));


       // return the extension
       return extension;
    }
}
