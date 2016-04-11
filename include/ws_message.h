#ifndef WS_MESSAGE_H
#define WS_MESSAGE_H

#include <phpcpp.h>
#include <string>
#include "ws_const.h"
#include "ws_header.h"
#include <boost/variant.hpp>
#include <boost/any.hpp>
#include "param.h"

using namespace std;

/*
 *
 */
class WSMessage : public Php::Base {

public:

	bool _wsa_must_understand = false;

	std::string _content_type;

	std::string _attachment_content_type = "application/octet-stream";

	std::string _rest_content_type;

	/**
	 * A URI representing the endpoint URI of the service to be consumed.
	 * This must be present on the client side. In case WS-Addressing is in use,
	 * this will be used as the WSA To header.
	 *
	 * Additionaly in case of addressing, you can set reference parameters using the following
	 * array of options as following sample code.
	 */
	Param *_endpoint = nullptr;

	Param *_reply_to = nullptr;

	Param *_from     = nullptr;

	Param *_fault    = nullptr;

	Param *_action   = nullptr;

	Param *_payload  = nullptr;

	string _response;

	/***
	 *
	 */
	std::map<std::string,std::string> _attachments_map;

	std::vector<WSHeader *> _wsheader_map;

	/**
	 * c++ ctor
	 */
	WSMessage();

	/**
	 * c++ dtor
	 */
	virtual ~WSMessage();


	bool hasAction();

	bool hasPayload();

	bool hasFault();

	bool hasFrom();

	bool hasReply();

	bool hasEndpoint();

	bool hasHeaders();

	/*
	 * WSMessage PHP __ctor
	 *
	 * {{{ proto WSMessage::__construct(mixed payload[, array properties])
	 */
	void __construct(Php::Parameters &params);


	Php::Value set_payload(Php::Parameters &params);

	Php::Value set_endpoint(Php::Parameters &params);

	Php::Value set_action(Php::Parameters &params);

	Php::Value set_from(Php::Parameters &params);

	Php::Value set_reply_to(Php::Parameters &params);

	Php::Value set_fault(Php::Parameters &params);

	Php::Value set_must_understand(Php::Parameters &params);

	Php::Value set_attachments(Php::Parameters &params);

	Php::Value set_attachment_content_type(Php::Parameters &params);

	Php::Value set_headers(Php::Parameters &params);

	Php::Value set_rest_content_type(Php::Parameters &params);

	template <typename T>
	T get_headers();

	/**
	 * Php Getter
	 */
	Php::Value get_from();

	Php::Value get_reply_to();

	Php::Value get_fault();

	Php::Value is_must_understand();

	Php::Value get_attachements();

	Php::Value get_attachement_content_type();

	Php::Value get_rest_content_type();

	Php::Value get_debug();



	/**
	 *
	 */
	Param * getPayload();

	Param * getParamByName(const std::string name);

	Php::Value getResponse();
};

#endif
