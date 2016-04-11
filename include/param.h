#ifndef PARAM_H
#define PARAM_H

#include <phpcpp.h>
#include <iostream>
#include <boost/variant.hpp>
#include <boost/variant/get.hpp>
#include <boost/any.hpp>

/**
 * This can be seen as an help for parameters
 */
class Param {

	typedef boost::variant<boost::blank, std::string, std::map<std::string,std::vector<std::string>>> P;

	typedef std::map<std::string,std::vector<std::string>> A;

private:

	P _param;

public:

	/**
	 * ctor
	 */
	Param();
	Param(Php::Value value);

	/**
	 * dtor
	 */
	virtual ~Param();

	/**
	 * Is the param is array
	 *
	 * @access public
	 * @return boolean
	 */
	bool is_array();

	/**
	 * Is the param is string
	 *
	 * @access public
	 * @return boolean
	 */
	bool is_string();

	/**
	 * Is the param exist
	 *
	 * @access public
	 * @return boolean
	 */
	bool exist();

	/**
	 * Push
	 */
	void add(Php::Value value);

	/**
	 * Template for get() method
	 *
	 * @access public
	 * @return T _param
	 */
	template <typename T>
	T get();
};

#endif
