#ifndef WS_HEADER_H
#define WS_HEADER_H

#include <phpcpp.h>
#include <string>
#include <boost/variant.hpp>
#include <boost/any.hpp>
#include <boost/assign.hpp>
#include <map>
#include "param.h"

using namespace std;
using namespace boost;
using namespace boost::assign;

class WSHeader :public Php::Base {

public:

    typedef boost::variant<std::string, std::vector<WSHeader *>> P;

    /*
     *
     */
    std::string _ns;

    /*
     *
     */
    std::string _prefix;

    /*
     *
     */
    std::string _name;

    /*
     *
     */
    bool _must_understand = false;

    /*
     *
     */
    P _header;

    /*
     *
     */
    int _role = -1;

	/**
	 * c++ ctor
	 */
	WSHeader();

	/**
	 * c++ dtor
	 */
	virtual ~WSHeader();

	/**
	 * set the soap Header's namespace uri
	 */
	Php::Value setNs(Php::Parameters &params);

	/**
	 * set the soap Header's namespace prefix
	 */
	Php::Value setPrefix(Php::Parameters &params);

	/**
	 * set the soap Header element's localname
	 */
	Php::Value setName(Php::Parameters &params);

	/**
	 * set a string or an array of WSHeader objecs
	 */
	Php::Value setData(Php::Parameters &params);

	/**
	 * specify whether to add mustUnderstand attribute to the Soap Header or not
	 */
	Php::Value setMustUnderstand(Php::Parameters &params);

	/**
	 * set the soap Header role value.
	 */
	Php::Value setRole(Php::Parameters &params);

	/**
	 * get data
	 */
	Php::Value getData();

	/**
	 * get role
	 */
	Php::Value getRole();

	/**
	 * get name
	 */
	Php::Value getName();

	/**
	 * get prefix
	 */
	Php::Value getPrefix();

	/**
	 * get ns
	 */
	Php::Value getNs();

	/**
	 * is must understand
	 */
	Php::Value hasMustUnderstand();

	/**
	 * PHP __ctor
	 *
	 * WSHeader::__construct(array[0,1])
	 */
	void __construct();

	/**
	 *
	 */
	bool hasNamespace();

	/*
	 *
	 */
	bool hasPrefix();

	/*
	 *
	 */
    bool hasName();

    /*
     *
     */
    bool hasHeaderObjects();

    /*
     *
     */
    bool hasRole();

    /*
     *
     */
    std::string getRoleAttributeBySoapVersion(int v);

    /*
     *
     */
    std::string getRoleNamespace();

    /*
     *
     */
    template <typename T>
    T getHeaders();
};

#endif
