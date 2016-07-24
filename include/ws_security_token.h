#ifndef WS_SECURITY_TOKEN_H
#define WS_SECURITY_TOKEN_H

#include <phpcpp.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/stat.h>

class WSSecurityToken : public Php::Base {

private:

	std::string parsePEMByFile(const std::string file);

public:

	int _ttl = 360;

	std::string _user;
	std::string _certificate;
	std::string _password_type;
	std::string _password;
	std::string _private_key;
	std::string _receiver_certificate;
	std::string _password_callback;
	std::string _replay_detection_callback;
	std::string _sct_store_callback;

	//mixed
	std::string _password_callback_data;
	//mixed
	std::string _replay_detection_dallback_data;
	//mixed
	std::string _sct_get_callback;
	//mixed
	std::string _sct_delete_callback;
	//mixed
	std::string _sct_callback_data;

	WSSecurityToken();

	virtual ~WSSecurityToken();

	void __construct();

	bool hasUser();

	bool hasCertificate();

	bool hasReceiverCertificate();

	bool hasPrivateKey();

	bool hasPassword();

	bool hasPasswordType();

	bool isFileExist(const std::string& name);

	Php::Value set_user(Php::Parameters &params);

	Php::Value set_certificate(Php::Parameters &params);

	Php::Value set_password_type(Php::Parameters &params);

	Php::Value set_password(Php::Parameters &params);

	Php::Value set_private_key(Php::Parameters &params);

	Php::Value set_receiver_certificate(Php::Parameters &params);

	Php::Value set_ttl(Php::Parameters &params);

	Php::Value set_password_callback(Php::Parameters &params);

	Php::Value set_replay_detection_callback(Php::Parameters &params);

	Php::Value set_replay_detection_callback_data(Php::Parameters &params);

	Php::Value set_sct_store_callback(Php::Parameters &params);

	Php::Value set_sct_get_callback(Php::Parameters &params);

	Php::Value set_sct_delete_callback(Php::Parameters &params);

	Php::Value set_sct_callback_data(Php::Parameters &params);
};

#endif
