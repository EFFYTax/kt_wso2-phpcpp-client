#include "ws_header.h"
#include <iostream>

/**
 * c++ ctor
 */
WSHeader :: WSHeader
(){};

/**
 * c++ dtor
 */
WSHeader :: ~WSHeader
(){};

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
Php::Value WSHeader :: setNs(Php::Parameters &params) {

	Php::Value self(this);
	Php::Value p =  params[0];

	if(!p.isString())
		throw Php::Exception("WSHeader::$ns expected to be a string");

	_ns = p.stringValue();

	self["ns"] = p;

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
Php::Value WSHeader :: setPrefix(Php::Parameters &params) {

	Php::Value self(this);
	Php::Value p =  params[0];

	if(!p.isString())
		throw Php::Exception("WSHeader::$prefix expected to be a string");

	_prefix = p.stringValue();

	self["prefix"] = p;

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
Php::Value WSHeader :: setName(Php::Parameters &params) {

	Php::Value self(this);
	Php::Value p =  params[0];

	if(!p.isString())
		throw Php::Exception("WSHeader::$name expected to be a string");

	_name = p.stringValue();

	self["name"] = p;

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
Php::Value WSHeader :: setData(Php::Parameters &params) {

	Php::Value self(this);
	Php::Value p =  params[0];

	if(!p.isString() && !p.isArray())
		throw Php::Exception("WSHeader::data expected to be a string or array of WSHeader object");

	//Ensure this is array of object<WSHeader> and hydrate vector
	if(p.isArray())
	{
		std::vector<WSHeader *> wsHeaders;

		for(auto &it: p)
		{
			if(!it.second.isObject() && !it.second.instanceOf("KTWS\\WSHeader"))
			{
				throw Php::Exception("Unpected parameters in array of <KTWS\\WSHeader>");
			}
			WSHeader * wsheader = ((WSHeader *)it.second.implementation());

			wsHeaders.push_back(wsheader);
		}
		_header = wsHeaders;
	}
	else
	{
	    _header = p.stringValue();
	}
	self["data"] = p;

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
Php::Value WSHeader :: setMustUnderstand(Php::Parameters &params) {

	Php::Value self(this);
	Php::Value p =  params[0];

	if(!p.isBool()) {
		throw Php::Exception("Unexpected parameter for must understand, Must be a boolean");
	}

	_must_understand = p;

	self["mustUnderstand"] = p;

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
Php::Value WSHeader :: setRole(Php::Parameters &params) {

	Php::Value self(this);
	Php::Value p =  params[0];

	if(!p.isNumeric()) {
		throw Php::Exception("Unexpected parameter for role, Must be numeric");
	}

	int role = p.numericValue();

	if(role != 1 && role != 0 && role != 3) {
	    throw Php::Exception("Unexpected parameter for role - must be 0 or 1 or 3");
	}
	_role = role;

	self["role"] = role;

	return this;
}

/**
 * Getter Php WSHeader
 */
Php::Value WSHeader :: getNs()     		    { return _ns; };
Php::Value WSHeader :: getPrefix() 		    { return _prefix; };
Php::Value WSHeader :: getName()   		    { return _name; };
Php::Value WSHeader :: hasMustUnderstand()  { return _must_understand; };
Php::Value WSHeader :: getRole()   		    { return _role; };


/**
 * Get Data array<WSClient> || <string>
 *
 */
Php::Value WSHeader :: getData() {

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
	    std::vector<WSHeader *> aWSHeader =  boost::get<std::vector<WSHeader *>>(_header);

		return aWSHeader;
	}
	else
	{
		return "";
	}
};

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

bool WSHeader :: hasRole()
{
    return _role == -1 ? false : true;
}

/*
 * Get the role attribute by SoapVersion
 */
std::string WSHeader :: getRoleAttributeBySoapVersion(int v)
{
    //AXIOM_SOAP12 = 2
    //AXIOM_SOAP11 = 1
    if(!v)
        throw std::invalid_argument("need the soap version");

    return v == 1 ? "actor" : "role";
}

/*
 * Get the Role namespace from internal parameter
 */
std::string WSHeader :: getRoleNamespace() {

    map<int, string> map = map_list_of
           (0, "http://www.w3.org/2003/05/soap-envelope/role/next")
           (1, "http://www.w3.org/2003/05/soap-envelope/role/none")
           (3, "http://www.w3.org/2003/05/soap-envelope/role/next");

    return map[_role];
}

/**
 * Get Headers specialisation
 */
template <typename T>
T WSHeader :: getHeaders(){};

/**
 *
 */
template <>
std::vector<WSHeader *> WSHeader :: getHeaders()
{
    return  boost::get<std::vector<WSHeader *>>(_header);
}

/**
 *
 */
template <>
std::string WSHeader :: getHeaders()
{
    return boost::get<std::string>(_header);
}

