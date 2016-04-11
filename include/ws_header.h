#ifndef WS_HEADER_H
#define WS_HEADER_H

#include <phpcpp.h>
#include <string>
#include <boost/variant.hpp>
#include <boost/any.hpp>
#include "param.h"

class WSHeader :public Php::Base {

private:

	typedef boost::variant<std::string, std::vector<WSHeader *>> P;

public:

	std::string _ns;
	std::string _prefix;
	std::string _name;

	bool _must_understand = false;

	P _header;

	int _role = 1;

	/**
	 * c++ ctor
	 */
	WSHeader();

	/**
	 * c++ dtor
	 */
	virtual ~WSHeader();

	bool hasNamespace();
	bool hasPrefix();
	bool hasName();
	bool hasHeaderObjects();

	template <typename T>
	T get_headers();

	/**
	 * set the soap Header's namespace uri
	 */
	Php::Value set_ns(Php::Parameters &params);

	/**
	 * set the soap Header's namespace prefix
	 */
	Php::Value set_prefix(Php::Parameters &params);

	/**
	 * set the soap Header element's localname
	 */
	Php::Value set_name(Php::Parameters &params);

	/**
	 * set a string or an array of WSHeader objecs
	 */
	Php::Value set_data(Php::Parameters &params);

	/**
	 * specify whether to add mustUnderstand attribute to the Soap Header or not
	 */
	Php::Value set_must_understand(Php::Parameters &params);

	/**
	 * set the soap Header role value.
	 */
	Php::Value set_role(Php::Parameters &params);

	/**
	 * get data
	 */
	Php::Value get_data();

	/**
	 * get role
	 */
	Php::Value get_role();

	/**
	 * get name
	 */
	Php::Value get_name();

	/**
	 * get prefix
	 */
	Php::Value get_prefix();

	/**
	 * get ns
	 */
	Php::Value get_ns();

	/**
	 * is must understand
	 */
	Php::Value is_must_understand();

	/**
	 * PHP __ctor
	 *
	 * WSHeader::__construct(array[0,1])
	 */
	void __construct();
};

#endif
