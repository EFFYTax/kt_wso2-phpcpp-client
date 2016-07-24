#include "ws_security_token.h"

/**
 * c++ ctor
 */
WSSecurityToken::WSSecurityToken
(){};

/**
 * c++ dtor
 */
WSSecurityToken::~WSSecurityToken
(){};

/**
 * PHP __ctor
 *
 * WSSecurityToken::__construct()
 */
void WSSecurityToken::__construct
(){};


/**
 * Is the file exist ?
 */
bool WSSecurityToken :: isFileExist (const std::string& name) {
  struct stat buffer;
  return (stat (name.c_str(), &buffer) == 0);
};

/**
 * Username
 *
 * @param <string>
 */
Php::Value WSSecurityToken :: set_user(Php::Parameters &params) {

	Php::Value self(this);
	Php::Value p = params[0];

	if(!p.isString())
		throw Php::Exception("Unexpected::$user Must be a string");

	_user = p.stringValue();

	self["user"] = p;

	return this;
}

/**
 * Parse PEM File and return certificate key or
 *
 * throw Php exception if bad format detected.
 *
 * @access private
 * @throw  Php::Exception
 * @param  <string> as filename
 * @return <string> as parsed pem key
 */
std::string WSSecurityToken :: parsePEMByFile(const std::string filename)
{
	static std::string HEADER = "-----BEGIN";
	static std::string FOOTER = "-----END";

	std::ifstream t(filename);
	std::stringstream buffer;
	std::string v;
	size_t pos1, pos2;

	buffer << t.rdbuf();

	pos1 = buffer.str().find(HEADER);
	if(pos1 == std::string::npos)
		throw Php::Exception("The cert supplied does not have a PEM Header");

	pos2 = buffer.str().find(FOOTER, pos1+1);
	if(pos2 == std::string::npos)
		throw Php::Exception("The cert supplied does not have a PEM Footer");

	size_t head_line = buffer.str().find_first_of( "\n",  pos1 + HEADER.length());
	v = buffer.str().erase(pos1, head_line - pos1 + 1);
	pos2 = v.find(FOOTER, 0);
	size_t foot_line = v.find_first_of( "\n",  pos2 + FOOTER.length());
	v = v.erase(pos2, foot_line - pos2 + 1);

	return v;
}

/**
 * producer's certificate (public key).
 *
 * This should be loaded using the function ws_get_cert_from_file().
 * This key is send to the receiver with security headers.
 *
 * @param <string>
 */
Php::Value WSSecurityToken :: set_certificate(Php::Parameters &params) {

	Php::Value self(this);
	Php::Value p = params[0];

	if(!p.isString())
		throw Php::Exception("Unexpected::$certificate Must be a string");

	if(!isFileExist(p.stringValue()))
		throw Php::Exception("Unexpected::$certificate file does not exist");

	_certificate =  parsePEMByFile(p.stringValue());

	self["certificate"] = _certificate;

	return this;
}

/**
 * Password type to be used in UsernameToken
 *
 * password	string	string	none
 *
 * @param <string>
 */
Php::Value WSSecurityToken :: set_password_type(Php::Parameters &params) {

	Php::Value self(this);
	Php::Value p = params[0];

	if(!p.isString())
		throw Php::Exception("Unexpected::$password_type Must be a string");

	_password_type = p.stringValue();

	self["password_type"] = p;

	return this;
}

/**
 * Producer's password to be used for UsernameToken
 */
Php::Value WSSecurityToken :: set_password(Php::Parameters &params) {

	Php::Value self(this);
	Php::Value p = params[0];

	if(!p.isString())
		throw Php::Exception("Unexpected::$password Must be a string");

	_password = p.stringValue();

	self["password"] = p;

	return this;
}

/**
 * Producer's private key. Should be loaded using the function
 * ws_get_key_from_file()
 */
Php::Value WSSecurityToken :: set_private_key(Php::Parameters &params) {

	Php::Value self(this);
	Php::Value p = params[0];

	if(!p.isString())
		throw Php::Exception("Unexpected::$private_key Must be a string");

	_private_key = parsePEMByFile(p.stringValue());

	self["private_key"] = _private_key;

	return this;
}

/**
 * Receiver's certificate. Should be loaded from the function
 * ws_get_cert_from_file()
 */
Php::Value WSSecurityToken :: set_receiver_certificate(Php::Parameters &params) {

	Php::Value self(this);
	Php::Value p = params[0];

	if(!p.isString())
		throw Php::Exception("Unexpected::$receiver_certificate Must be a string");

	_receiver_certificate = parsePEMByFile(p.stringValue());

	self["receiver_certificate"] = _receiver_certificate;

	return this;
}

/**
 * The time period within which the message would be valid
 * (time to live) in seconds
 *
 */
Php::Value WSSecurityToken :: set_ttl(Php::Parameters &params) {

	Php::Value self(this);
	Php::Value p = params[0];

	if(!p.isString())
		throw Php::Exception("Unexpected::$ttl Must be a string");

	_ttl = p;

	self["ttl"] = p;

	return this;
}

/**
 * Name of the password callback function, which is a user defined
 * function that accepts a username and returns the
 * corresponding password. The signature of this function should be like
 * string function_name(string username[,mixed args]).
 *
 * The args is an optional argument in case the user was
 * to pass additional data to the function that would be necessary
 * to obtain the password.
 */
Php::Value WSSecurityToken :: set_password_callback(Php::Parameters &params) {

	Php::Value self(this);
	Php::Value p = params[0];

	if(!p.isString())
		throw Php::Exception("Unexpected::$password_callback Must be a string");

	_password_callback = p.stringValue();

	self["password_callback"] = p;

	return this;
}

/**
 * Name of the password callback function,
 * which is a user defined function that accepts a username and returns the corresponding password.
 * The signature of this function should be
 *
 * like string function_name(string username[,mixed args]).
 *
 * The args is an optional argument in case the user was to pass
 * additional data to the function that would be necessary
 * to obtain the password.
 */
Php::Value WSSecurityToken :: set_replay_detection_callback(Php::Parameters &params) {

	Php::Value self(this);
	Php::Value p = params[0];

	if(!p.isString())
		throw Php::Exception("Unexpected::$replay_detection_callback Must be a string");

    _replay_detection_callback = p.stringValue();

	self["replay_detection_callback"] = p;

	return this;
}

/**
 *
 */
Php::Value WSSecurityToken :: set_replay_detection_callback_data(Php::Parameters &params) {

	Php::Value self(this);
	Php::Value p = params[0];

	if(!p.isString())
		throw Php::Exception("Unexpected::$replay_detection_dallback_data Must be a string");

	_replay_detection_dallback_data = p.stringValue();

	self["replay_detection_dallback_data"] = p;

	return this;
}

Php::Value WSSecurityToken :: set_sct_store_callback(Php::Parameters &params) {

	Php::Value self(this);
	Php::Value p = params[0];

	if(!p.isString())
		throw Php::Exception("Unexpected::$sct_store_callback Must be a string");

	_sct_store_callback = p.stringValue();

	self["sct_store_callback"] = p;

	return this;
}

Php::Value WSSecurityToken :: set_sct_get_callback(Php::Parameters &params) {

	Php::Value self(this);
	Php::Value p = params[0];

	if(!p.isString())
		throw Php::Exception("Unexpected::$sct_get_callback Must be a string");

	_sct_get_callback = p.stringValue();

	self["sct_get_callback"] = p;

	return this;
}

Php::Value WSSecurityToken :: set_sct_delete_callback(Php::Parameters &params) {

	Php::Value self(this);
	Php::Value p = params[0];

	if(!p.isString())
		throw Php::Exception("Unexpected::$sct_delete_callback Must be a string");

	_sct_delete_callback = p.stringValue();

	self["sct_delete_callback"] = p;

	return this;
}

Php::Value WSSecurityToken :: set_sct_callback_data(Php::Parameters &params) {

	Php::Value self(this);
	Php::Value p = params[0];

	if(!p.isString())
		throw Php::Exception("Unexpected::$sct_callback_data Must be a string");

	_sct_callback_data = p.stringValue();

	self["sct_callback_data"] = p;

	return this;
}

/**
 *
 */
bool WSSecurityToken :: hasUser()
{
	return _user.empty() ? false : true;
}

bool WSSecurityToken :: hasCertificate()
{
	return _certificate.empty() ? false : true;
}

bool WSSecurityToken :: hasReceiverCertificate()
{
	return _receiver_certificate.empty() ? false : true;
}

bool WSSecurityToken :: hasPrivateKey()
{
	return _private_key.empty() ? false : true;
}

bool WSSecurityToken :: hasPassword()
{
	return _password.empty() ? false : true;
}

bool WSSecurityToken :: hasPasswordType()
{
    return _password_type.empty() ? false : true;
}







