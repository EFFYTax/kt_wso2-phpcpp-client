#include "ws_header.h"
#include "ws_common.h"
#include <iostream>

/**
 * c++ ctor
 */
WSHeader :: WSHeader
(){};

/**
 * c++ dtor
 */
WSHeader :: ~WSHeader(){
};

/**
 * PHP Ctor
 *
 * WSHeader::__contruct(array[0,1])
 */
void WSHeader :: __construct(){};

/**
 * Set the soap Header's namespace uri
 *
 * @param  Php::Parameters params <string>
 * @return Php::Value<WSClient>
 * @access public
 * @throw  Php::Exception
 */
Php::Value WSHeader :: set_ns(Php::Parameters &params) {

	Php::Value self(this);
	Php::Value p =  params[0];

	if(!p.isString())
		throw Php::Exception("WSHeader::$ns expected to be a string");

	_ns = p.stringValue();

	self[WSF_HEADER_NS] = _ns;

	return this;
}

/**
 * set the soap Header's namespace prefix
 *
 * @param  Php::Parameters params <string>
 * @return Php::Value<WSClient>
 * @access public
 * @throw  Php::Exception
 */
Php::Value WSHeader :: set_prefix(Php::Parameters &params) {

	Php::Value self(this);
	Php::Value p =  params[0];

	if(!p.isString())
		throw Php::Exception("WSHeader::$prefix expected to be a string");

	_prefix = p.stringValue();

	self[WSF_HEADER_PREFIX] = _prefix;

	return this;
}

/**
 * set the soap Header element's localname
 *
 * @param  Php::Parameters params <string>
 * @return Php::Value<WSClient>
 * @access public
 * @throw  Php::Exception
 */
Php::Value WSHeader :: set_name(Php::Parameters &params) {

	Php::Value self(this);
	Php::Value p =  params[0];

	if(!p.isString())
		throw Php::Exception("WSHeader::$name expected to be a string");

	_name = p.stringValue();

	self[WSF_HEADER_LOCALNAME] = _name;

	return this;
}

/**
 * set a string or an array of WSHeader objecs
 *
 * @param  Php::Parameters params
 * @return Php::Value<WSClient>
 * @access public
 * @throw  Php::Exception
 */
Php::Value WSHeader :: set_data(Php::Parameters &params) {

	Php::Value self(this);
	Php::Value p =  params[0];

	if(!p.isString() && !p.isArray())
		throw Php::Exception("WSHeader::data expected to be a string or array of WSHeader object");

	//Ensure this is array of object<WSHeader> and hydrate vector
	if(p.isArray())
	{
		std::vector<WSHeader *> v;
		for(auto &it: p)
		{
			if(!it.second.isObject() && !it.second.instanceOf("KTWS\\WSHeader"))
			{
				throw Php::Exception("Unpected parameters in array of <WSHeader>");
			}

			WSHeader *object = (WSHeader *)it.second.implementation();
			v.push_back(object);
		}
		_header = v;
	}
	else
	{
		_header = p.stringValue();
	}
	self[WSF_HEADER_DATA] = p;

	return this;
}

/**
 * specify whether to add mustUnderstand attribute to the Soap Header or not
 *
 * @param  Php::Parameters params
 * @return Php::Value<WSClient>
 * @access public
 * @throw  Php::Exception
 */
Php::Value WSHeader :: set_must_understand(Php::Parameters &params) {

	Php::Value self(this);
	Php::Value p =  params[0];

	if(!p.isBool()) {
		throw Php::Exception("Unexpected parameter for must understand, Must be a boolean");
	}
	_must_understand = p;

	self[WSF_HEADER_MUST_UNDERSTAND] = _must_understand;

	return this;
}

/**
 * set the soap Header role value.
 *
 * @param  Php::Parameters params
 * @return Php::Value<WSClient>
 * @access public
 * @throw  Php::Exception
 */
Php::Value WSHeader :: set_role(Php::Parameters &params) {

	Php::Value self(this);
	Php::Value p =  params[0];

	if(!p.isNumeric()) {
		throw Php::Exception("Unexpected parameter for role, Must be numeric");
	}
	_role = p;

	self[WSF_HEADER_ROLE] = p;

	return this;
}

/**
 * Getter Php WSHeader
 */
Php::Value WSHeader :: get_ns()     		{ return _ns; };
Php::Value WSHeader :: get_prefix() 		{ return _prefix; };
Php::Value WSHeader :: get_name()   		{ return _name; };
Php::Value WSHeader :: is_must_understand() { return _must_understand;};
Php::Value WSHeader :: get_role()   		{ return _role; };

/**
 * Get Data array<WSClient> || <string>
 *
 */
Php::Value WSHeader :: get_data() {

	if(_header.which() == 0)
	{
		if(boost::get<std::string>(_header).length() > 0)
		{
			return boost::get<std::string>(_header);
		}
		else
		{
			return "empty!";
		}
	}
	else if(_header.which() == 1)
	{
		return boost::get<std::vector<WSHeader *>>(_header);
	}
	else
	{
		return "empty!catch";
	}
};

/**
 *
 */
template <typename T>
T WSHeader :: get_headers(){};

/**
 *
 */
template <>
std::vector<WSHeader *> WSHeader :: get_headers()
{
	return boost::get<std::vector<WSHeader *>>(_header);
}

/**
 *
 */
template <>
std::string WSHeader :: get_headers()
{
	return boost::get<std::string>(_header);
}

/**
 * has Namesapce
 */
bool WSHeader :: hasNamespace()
{
	return _ns.empty() ? false : true ;
}

/**
 * has Prefix
 */
bool WSHeader :: hasPrefix()
{
	return _prefix.empty() ? false : true ;
}

/**
 * has Name
 */
bool WSHeader :: hasName()
{
	return _name.empty() ? false : true ;
}

/**
 * has array of header object ( data )
 */
bool WSHeader :: hasHeaderObjects()
{
	if( _header.which() == 1 )
	{
		std::vector<WSHeader *> v = boost::get<std::vector<WSHeader *>>(_header);
		if(v.size() > 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}
