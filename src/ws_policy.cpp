#include "ws_policy.h"

/**
 * c++ ctor
 */
WSPolicy::WSPolicy
(){};

/**
 * c++ dtor
 */
WSPolicy::~WSPolicy
(){};

/**
 * PHP __ctor
 *
 * WSPolicy::__construct()
 */
void WSPolicy::__construct
(){};

/**
 *
 */
Php::Value WSPolicy :: set_sign(Php::Parameters &params)
{
	Php::Value self(this);
	Php::Value p = params[0];

	if(!p.isBool())
		throw Php::Exception("Unexpected parameter for WSPolicy::$sign");

	_sign = p;

	self["sign"] = p;

	return this;
}

/**
 *
 */
Php::Value WSPolicy :: set_encrypt(Php::Parameters &params)
{
	Php::Value self(this);
	Php::Value p = params[0];

	if(!p.isBool())
		throw Php::Exception("Unexpected parameter for WSPolicy::$encrypt");

	_encrypt = p;

	self["encrypt"] = p;

	return this;
}

/**
 *
 */
Php::Value WSPolicy :: set_include_time_stamp(Php::Parameters &params)
{
	Php::Value self(this);
	Php::Value p = params[0];

	if(!p.isBool())
		throw Php::Exception("Unexpected parameter for WSPolicy::$include_time_stamp");

	_include_time_stamp = p;

	self["include_time_stamp"] = p;

	return this;
}

/**
 *
 */
Php::Value WSPolicy :: set_use_username_token(Php::Parameters &params)
{
	Php::Value self(this);
	Php::Value p = params[0];

	if(!p.isBool())
		throw Php::Exception("Unexpected parameter for WSPolicy::$use_username_token");

	_use_username_token = p;

	self["use_username_token"] = p;

	return this;
}

/**
 *
 */
Php::Value WSPolicy :: set_encrypt_signature(Php::Parameters &params)
{
	Php::Value self(this);
	Php::Value p = params[0];

	if(!p.isBool())
		throw Php::Exception("Unexpected parameter for WSPolicy::$encrypt_sign");

	_encrypt_signature = p;

	self["encrypt_signature"] = p;

	return this;
}

/**
 *
 */
Php::Value WSPolicy :: set_algorithm_suite(Php::Parameters &params)
{
	Php::Value self(this);
	Php::Value p = params[0];

	if(!p.isString())
		throw Php::Exception("Unexpected parameter for WSPolicy::$algorithm_suite");

	_algorithm_suite = p.stringValue();


	self["algorithm_suite"] = p;

	return this;
}

/**
 *
 */
Php::Value WSPolicy :: set_layout(Php::Parameters &params)
{
	Php::Value self(this);
	Php::Value p = params[0];

	if(!p.isString())
		throw Php::Exception("Unexpected parameter for WSPolicy::$layout");

	_layout = p.stringValue();

	self["layout"] = p;

	return this;
}

/**
 *
 */
Php::Value WSPolicy :: set_protection_order(Php::Parameters &params)
{
	Php::Value self(this);
	Php::Value p = params[0];

	if(!p.isString())
		throw Php::Exception("Unexpected parameter for WSPolicy::$protection_order");

	_protection_order = p.stringValue();

	self["protection_order"] = p;

	return this;
}

/**
 *
 */
Php::Value WSPolicy :: set_security_token_reference(Php::Parameters &params)
{
	Php::Value self(this);
	Php::Value p = params[0];

	if(!p.isString())
			throw Php::Exception("Unexpected parameter for WSPolicy::$security_token_reference");

	    _security_token_reference = p.stringValue();

		self["security_token_reference"] = p;

	return this;
}

/**
 *
 */
Php::Value WSPolicy :: set_binding(Php::Parameters &params)
{
	Php::Value self(this);
	Php::Value p = params[0];

	if(!p.isString())
		throw Php::Exception("Unexpected parameter for WSPolicy::$binding");

	_binding = p.stringValue();

	self["binding"] = p;

	return this;
}

/**
 *
 */
Php::Value WSPolicy :: set_policy(Php::Parameters &params)
{
	Php::Value self(this);
	Php::Value p = params[0];

	if(!p.isString())
		throw Php::Exception("Unexpected parameter for WSPolicy::$policy");

	_policy = p.stringValue();

	self["policy"] = p;


	return this;
}

/**
 *
 */
Php::Value WSPolicy :: set_in_message_policy(Php::Parameters &params)
{
	Php::Value self(this);
	Php::Value p = params[0];

	if(!p.isString() && !p.isArray())
		throw Php::Exception("Unexpected parameter for WSPolicy::$in_message_policy");

	if(p.isString())
	{
		_in_message_policy = p.stringValue();
	}
	else if(p.isArray())
	{
		std::map<std::string, std::string> map = p;

		_in_message_policy = map;
	}

	self["in_message_policy"] = p;

	return this;
}

/**
 *
 */
Php::Value WSPolicy :: set_out_message_policy(Php::Parameters &params)
{
	Php::Value self(this);
	Php::Value p = params[0];

	if(!p.isString() && !p.isArray())
		throw Php::Exception("Unexpected parameter for WSPolicy::$out_message_policy");

	if(p.isString())
	{
		_out_message_policy = p.stringValue();
	}
	else if(p.isArray())
	{
		std::map<std::string, std::string> map = p;

		_out_message_policy = map;
	}

	self["out_message_policy"] = p;

	return this;
}

/**
 * Has Policy stored as xml
 */
bool WSPolicy :: hasXMLPolicy()
{
	return _policy.length() > 0 ? true : false;
}

/**
 * Has Policy object include timestamp
 */
bool WSPolicy :: hasTimestamp()
{
	return _include_time_stamp;
}

/**
 * Has Username Token
 */
bool WSPolicy :: hasUsernameToken()
{
	return _use_username_token;
}

/**
 * Has Encrypted
 */
bool WSPolicy :: hasEncrypted()
{
	return _encrypt;
}

/**
 * Has algo suite
 */
bool WSPolicy :: hasAlgoSuite()
{
	return _algorithm_suite.length() > 0 ? true : false;
}

/**
 * Has signed policy
 */
bool WSPolicy :: hasSigned()
{
	return _sign;
}

/**
 * Has Security Token Reference
 */
bool WSPolicy :: hasSecurityTokenRef()
{
	return ( _security_token_reference.length() > 0 ) ? true : false;
}

/**
 *
 */
bool WSPolicy :: hasProtectionOrder()
{
	return ( _protection_order.length() > 0 ) ? true : false;
}

/**
 *
 */
bool WSPolicy :: hasSecurityBinding()
{
	return ( _binding.length() > 0 ) ? true : false;
}


/**
 *
 */
string WSPolicy :: getRampartTokenValue()
{
	return _rampart_token[_security_token_reference];
}

/**
 *
 */
Php::Value WSPolicy :: get_sign(Php::Parameters &params) 		   				{ return _sign; };
Php::Value WSPolicy :: get_encrypt(Php::Parameters &params) 		   			{ return _encrypt; };
Php::Value WSPolicy :: get_include_time_stamp(Php::Parameters &params) 			{ return _include_time_stamp; };
Php::Value WSPolicy :: get_use_username_token(Php::Parameters &params)			{ return _use_username_token;   };
Php::Value WSPolicy :: get_encrypt_signature(Php::Parameters &params) 			{ return _encrypt_signature; }
Php::Value WSPolicy :: get_algorithm_suite(Php::Parameters &params) 			{ return _algorithm_suite; };
Php::Value WSPolicy :: get_layout(Php::Parameters &params) 						{ return _layout; };
Php::Value WSPolicy :: get_protection_order(Php::Parameters &params) 			{ return _protection_order; };
Php::Value WSPolicy :: get_security_token_reference(Php::Parameters &params) 	{ return _security_token_reference; };
Php::Value WSPolicy :: get_binding(Php::Parameters &params) 					{ return _binding; };
Php::Value WSPolicy :: get_policy(Php::Parameters &params) 						{ return _policy; };

