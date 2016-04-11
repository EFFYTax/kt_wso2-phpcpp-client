#ifndef WS_POLICY_H
#define WS_POLICY_H

#include <phpcpp.h>
#include <string>
#include <iostream>
#include <boost/variant.hpp>
#include <boost/any.hpp>
#include <boost/assign.hpp>

using namespace std;
using namespace boost;
using namespace boost::assign;

class WSPolicy
		:public Php::Base {

	typedef variant<string, map<string, string>> T;

private:

public:

	//bool
	bool _sign 							= false;
	bool _encrypt 						= false;
	bool _include_time_stamp 			= false;
	bool _use_username_token 			= false;
	bool _encrypt_signature 			= false;

	//string
	string _algorithm_suite 			= "Basic256Rsa15";
	string _layout 						= "Strict";
	string _protection_order 			= "SignBeforeEncrypt";
	string _security_token_reference 	= "KeyIdentifier";
	string _binding 					= "Asymmetric";
	string _policy;

	//mixed string|array
	T _in_message_policy;
	T _out_message_policy;

	//Rampart token value mapping
	map<string, string> _rampart_token = map_list_of
	("IssuerSerial",  "RequireIssuerSerialReference")
	("KeyIdentifier", "RequireKeyIdentifierReference")
	("EmbeddedToken", "RequireEmbeddedTokenReference")
	("Thumbprint",    "RequireThumbprintReference");

	/**
	 * c++ ctor
	 */
	WSPolicy();

	/**
	 * c++ dtor
	 */
	virtual ~WSPolicy();

	/**
	 * PHP __ctor
	 *
	 * WSPolicy::__construct()
	 */
	void __construct();

	/**
	 *
	 */
	bool hasXMLPolicy();

	/**
	 *
	 */
	bool hasTimestamp();

	/**
	 *
	 */
	bool hasUsernameToken();

	/**
	 *
	 */
	bool hasEncrypted();

	/**
	 *
	 */
	bool hasAlgoSuite();

	/**
	 *
	 */
	bool hasSigned();

	/**
	 *
	 */
	bool hasSecurityTokenRef();

	/**
	 *
	 */
	bool hasProtectionOrder();

	/**
	 *
	 */
	bool hasSecurityBinding();
	/**
	 *
	 */
	string getRampartTokenValue();

	/**
	 * Php Setter
	 */
	Php::Value set_sign(Php::Parameters &params);

	Php::Value set_encrypt(Php::Parameters &params);

	Php::Value set_include_time_stamp(Php::Parameters &params);

	Php::Value set_use_username_token(Php::Parameters &params);

	Php::Value set_encrypt_signature(Php::Parameters &params);

	Php::Value set_algorithm_suite(Php::Parameters &params);

	Php::Value set_layout(Php::Parameters &params);

	Php::Value set_protection_order(Php::Parameters &params);

	Php::Value set_security_token_reference(Php::Parameters &params);

	Php::Value set_binding(Php::Parameters &params);

	Php::Value set_policy(Php::Parameters &params);

	Php::Value set_in_message_policy(Php::Parameters &params);

	Php::Value set_out_message_policy(Php::Parameters &params);

	/**
	 * Php Getter
	 */
	Php::Value get_sign(Php::Parameters &params);

	Php::Value get_encrypt(Php::Parameters &params);

	Php::Value get_include_time_stamp(Php::Parameters &params);

	Php::Value get_use_username_token(Php::Parameters &params);

	Php::Value get_encrypt_signature(Php::Parameters &params);

	Php::Value get_algorithm_suite(Php::Parameters &params);

	Php::Value get_layout(Php::Parameters &params);

	Php::Value get_protection_order(Php::Parameters &params);

	Php::Value get_security_token_reference(Php::Parameters &params);

	Php::Value get_binding(Php::Parameters &params);

	Php::Value get_policy(Php::Parameters &params);

	Php::Value get_in_message_policy(Php::Parameters &params);

	Php::Value get_out_message_policy(Php::Parameters &params);
};

#endif
