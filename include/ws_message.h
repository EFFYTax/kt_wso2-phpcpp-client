#ifndef WS_MESSAGE_H
#define WS_MESSAGE_H

#include <phpcpp.h>
#include <string>
#include "axis2_message.h"
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

private:

    /*
     * Axis2Message, internal Axis2Message pointer holding the whole WSMessage logic
     */
    std::shared_ptr<Axis2Message> _axis2_message;

public:

    /**
	 * c++ ctor
	 */
	WSMessage();

	/**
	 * c++ dtor
	 */
	virtual ~WSMessage();

	/*
	 * WSMessage PHP __ctor
	 *
	 * {{{ proto WSMessage::__construct(mixed payload[, array properties])
	 */
	void __construct(Php::Parameters &params);

	/*
	 * Get access to the internal axis2_message object
	 *
	 * @return std::shared_ptr<Axis2Message>
	 */
	std::shared_ptr<Axis2Message> getAxis2Message()
	{
	    std::shared_ptr<Axis2Message> _p = _axis2_message;

	    return _p;
	}

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



	/**
	 * Php Getter
	 */
	Php::Value get_payload();
	Php::Value get_from();
	Php::Value get_reply_to();
	Php::Value get_fault();
	Php::Value get_action();
	Php::Value get_endpoint();
	Php::Value is_must_understand();
	Php::Value get_attachements();
	Php::Value get_attachement_content_type();
	Php::Value get_rest_content_type();
	Php::Value get_debug();
	Php::Value getResponse();

	template <typename T>
	T get_headers();


};

#endif
