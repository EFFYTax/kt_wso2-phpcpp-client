#include "ws_message.h"
#include <iostream>

/**
 * c++ ctor
 */
WSMessage::WSMessage
(){};

/**
 *  c++ dtor
 */
WSMessage :: ~WSMessage(){};

/**
 * Request payload being sent or being received
 *
 * The payload is expected to be in valid XML format as expected by the service.
 *
 * @params <Php::Parameters> as valid xml string payload
 * @return <WSMessage>
 * @throw  <Php::Exception>
 */

Php::Value WSMessage :: set_payload(Php::Parameters &params) {

	Php::Value self(this);
	Php::Value p = params[0];

	if(!p.isString())
		throw Php::Exception("Unexpected param for WSMessage::$payload, must be a string");

	_payload = new Param(p);

	self["payload"] = p;

	return this;
};

/**
 * A URI representing the endpoint URI of the service to be consumed.
 * This must be present on the client side. In case WS-Addressing is in use,
 * this will be used as the WSA To header. Additionaly in case of addressing,
 * you can set reference parameters using the following array of options
 * as following sample code.
 *
 * array("address"=>"to URL", "referenceParameters"=>array of xml strings)
 *
 * "referenceParameters"  are addressing options.
 */
Php::Value WSMessage :: set_endpoint(Php::Parameters &params) {

	Php::Value self(this);
	Php::Value p = params[0];

	if(!p.isString() && !p.isArray())
		throw Php::Exception("Unexpected param for WSMessage::$to, must be a string or array");

	Param *param = new Param();
	param->add(p);

	_endpoint = param;

	self["endpoint"] = p;

	return this;
};

/**
 * xs:anyURI
 *
 * The WS-A action to use when constructing the WS-A headers.
 * A URI representing the WSA action that helps uniquely identify
 * the operation to be invoked. This is also used as the SOAP Action regardless of
 * whether the useWSA option is specified or not.
 *
 * @params <Php::Parameters> as a string reprenseting the URI.
 * @return <WSMessage>
 * @throw  <Php::Exception>
 */
Php::Value WSMessage :: set_action(Php::Parameters &params) {

	Php::Value self(this);
	Php::Value p = params[0];

	if(!p.isString())
		throw Php::Exception("Unexpected param for WSMessage::$action, must be a string");

	Param *param = new Param();
	param->add(p);
	_action = param;

	self[WSF_ACTION] = p;

	return this;
};

/**
 * WS-Addressing From.
 *
 * A URI indicating where the response or request came from (xs:anyURI).
 * You can set reference parameters using the following array of options.
 *
 * array("address"=>"from URL", "referenceParameters"=>array of xml strings)
 *
 * @params <Php::Parameters> mixed string|array
 * @return <WSMessage>
 * @throw  <Php::Exception>
 */
Php::Value WSMessage :: set_from(Php::Parameters &params) {

	Php::Value self(this);
	Php::Value p = params[0];

	if(!p.isString() && !p.isArray())
		throw Php::Exception("Unexpected param for WSMessage::$from, must be a string or array");

	Param *param = new Param();
	param->add(p);

	_from = param;

	self[WSF_FROM] = p;

	return this;
};

/**
 * WS-Addressing ReplyTo.
 *
 * WS-Addressing A URI indicating where the response should be sent to.
 * The default value is the anonymous URI (depending on the version).
 * You can set reference parameters using the following array of options.
 *
 * array("address"=>"replyTo URL", "referenceParameters"=>array of xml strings)
 *
 * @params <Php::Parameters> mixed string|array
 * @return <WSMessage>
 * @throw  <Php::Exception>
 */
Php::Value WSMessage :: set_reply_to(Php::Parameters &params) {

	Php::Value self(this);
	Php::Value p = params[0];

	if(!p.isString() && !p.isArray())
		throw Php::Exception("Unexpected param for WSMessage::$reply_to, must be a string or array");

	Param *param = new Param();
	param->add(p);

	_reply_to = param;

	self[WSF_REPLY_TO] = p;

	return this;
};

/**
 * WS-Addressing FaultTo.
 *
 * A URI indicating where the fault should be sent to in case of an error.
 * You can set reference parameters using the following array of options.
 *
 * array("address"=>"faultTo URL", "referenceParameters"=>array of xml strings)
 *
 * @params <Php::Parameters> mixed string|array
 * @return <WSMessage>
 * @throw  <Php::Exception>
 */
Php::Value WSMessage :: set_fault(Php::Parameters &params) {

	Php::Value self(this);
	Php::Value p = params[0];

	if(!p.isString() && !p.isArray())
		throw Php::Exception("Unexpected param for WSMessage::$fault_to, must be a string or array");

	Param *param = new Param();
	param->add(p);

	_fault = param;

	self[WSF_FAULT_TO] = p;

	return this;
};

/**
 * WSAMustUnderstand
 *
 * mustUndertsand attribute can be added to addressing headers by setting this option to TRUE.
 *
 * @params <Php::Parameters> boolean
 * @return <WSMessage>
 * @throw  <Php::Exception>
 *
 */
Php::Value WSMessage :: set_must_understand(Php::Parameters &params) {

	Php::Value self(this);
	Php::Value p = params[0];

	if(!p.isBool())
		throw Php::Exception("Unexpected param for WSMessage::$wsa_must_understand, must be a boolean");

	_wsa_must_understand = p;

	self[WSF_ADDR_MUST_UNDERSTAND] = p;

	return this;
};

/**
 * This is an associative array which maps the string value specified in the xml
 * to the matching binary string. If attachment caching is enabled, you can set the file name
 * instead of the binary string.
 *
 * @params <Php::Parameters> array
 * @return <WSMessage>
 * @throw  <Php::Exception>
 *
 */
Php::Value WSMessage :: set_attachments(Php::Parameters &params) {

	Php::Value self(this);

	Php::Value p = params[0];

	if(!p.isArray())
		throw Php::Exception("Unexpected param for WSMessage::$attachments, must be a array");

	_attachments_map = p;

	self[WSF_ATTACHMENTS] = p;

	return this;
};


/**
 * Specify a default MTOM content type to be used with MTOM attachments.
 * You can also specify xmlmime:contentType on the xop:Include element,
 * which will override this setting.
 *
 * @params <Php::Parameters> string
 * @return <WSMessage>
 * @throw  <Php::Exception>
 */
Php::Value WSMessage :: set_attachment_content_type(Php::Parameters &params) {

	Php::Value self(this);
	Php::Value p = params[0];

	if(!p.isString())
		throw Php::Exception("Unexpected param for WSMessage::$defaultAttachmentContentType, must be a string");

	_attachment_content_type = p.stringValue();

	self[WSF_DEFAULT_ATTACHEMENT_CONTENT_TYPE] = p;

	return this;
};

/**
 * When an array of WSHeader objects is set to the options array,
 * these headers will be present on the outgoing SOAP Envelope.
 *
 * @params <Php::Parameters>
 * @return <WSMessage>
 * @throw  <Php::Exception>
 */
Php::Value WSMessage :: set_headers(Php::Parameters &params) {

	Php::Value self(this);
	Php::Value p = params[0];

	if(p.isArray())
	{
		for (auto &it: p)
		{
			if(it.second.instanceOf("WSHeader"))
			{
				WSHeader *object = (WSHeader *)it.second.implementation();

				_wsheader_map.push_back(object);
			}
			else throw Php::Exception("Unexpected param for WSMessage::$inputHeaders, must be instance of WSHeader");
		}
	}
	else throw Php::Exception("Unexpected param for WSMessage::$inputHeaders, must array");

	self[WSF_INPUT_HEADERS] = p;

	return this;
};

/**
 * When exposing an operation in REST Style,you can get/set
 * media type of the content using this attribute of WSMessage
 *
 * @params <Php::Parameters>
 * @return <WSMessage>
 * @throw  <Php::Exception>
 */
Php::Value WSMessage :: set_rest_content_type(Php::Parameters &params)
{
	Php::Value self(this);
	Php::Value p = params[0];

	if(!p.isString())
		throw Php::Exception("Unexpected param for WSMessage::contentType, must be a string");

	_rest_content_type = p.stringValue();

	self["contentType"] = p;

	return this;
}

/**
 * Proxy to payload
 *
 * @access public
 * @return pointer to <Param>_payload
 */
Param * WSMessage :: getPayload()
{
	return _payload;
}

/**
 * Proxy to Param object by name
 *
 * TODO: Implement better ( template )
 */
Param * WSMessage :: getParamByName(const std::string name)
{
	if(name == WSF_REPLY_TO && dynamic_cast<Param *>(_reply_to) != NULL)
	{
		return _reply_to;
	}
	else if(name == WSF_FAULT_TO && dynamic_cast<Param *>(_fault) != NULL)
	{
		return _fault;
	}
	else if(name == WSF_FROM && dynamic_cast<Param *>(_from) != NULL)
	{
		return _from;
	}
	else
	{
		return nullptr;
	}
};

/**
 * Has Message embed headers
 *
 * @access public
 * @return bool
 */
bool WSMessage :: hasHeaders()
{
	return (!_wsheader_map.empty()) ? true : false;
}

/**
 * Template get_headers
 */
template <typename T>
T WSMessage :: get_headers() {};

/**
 * Specialisation for headers
 */
template <>
Php::Value WSMessage :: get_headers()
{
	return Php::Array(_wsheader_map);
}

/**
 * Specialisation for vector
 */
template <>
std::vector<WSHeader *> WSMessage :: get_headers()
{
	return _wsheader_map;
}


Php::Value WSMessage :: get_debug()
{
	return this;
}

/**
 *
 */
bool WSMessage :: hasAction()
{
	return (_action) ? true : false;
};

bool WSMessage :: hasPayload()
{
	return (_payload) ? true : false;
};

bool WSMessage :: hasFault()
{
	return (_fault) ? true : false;
};

bool WSMessage :: hasFrom()
{
	return (_from) ? true : false;
};

bool WSMessage :: hasReply()
{
	return (_reply_to) ? true : false;
};

bool WSMessage :: hasEndpoint()
{
	return (_endpoint) ? true : false;
};

Php::Value WSMessage :: getResponse()
{
	return _response;
}


/*
 * WSMessage PHP __ctor
 *
 * {{{ proto WSMessage::__construct(mixed payload[, array properties])
 */
void WSMessage::__construct(Php::Parameters &params)
{
};
