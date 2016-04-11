#include "param.h"

typedef boost::variant<std::string, std::map<std::string,std::vector<std::string>>> P;

typedef std::map<std::string,std::vector<std::string>> A;

Param :: Param(){};

/**
 * ctor
 */
Param :: Param(Php::Value value)
{
	add(value);
};

/**
 * dtor
 */
Param :: ~Param(){};

/**
 * Is the param is array
 *
 * @access public
 * @return boolean
 */
bool Param :: is_array()
{
	return _param.which() == 1 ? false : true ;
};

/**
 * Is the param is string
 *
 * @access public
 * @return boolean
 */
bool Param :: is_string()
{
	return _param.which() == 1 ? true : false;
};

/**
 * Is the param is empty
 *
 * @access public
 * @return boolean
 */
bool Param :: exist()
{
	bool exist = false;

	if(_param.which() == 1)
	{
		std::cout << "type 0 string" << std::endl;
		if(boost::get<std::string>(_param).length() > 0)
			exist = true;
	}
	else if(_param.which() == 2)
	{
		std::cout << "type 1 array" << std::endl;
		if(boost::get<A>(_param).size() > 0)
			exist = true;
	}

	return exist;
};

/**
 * Add param
 */
void Param :: add(Php::Value value)
{
	//php param is string
	if(value.isString())
	{
		_param = value.stringValue();
	}
	//php param is array
	else if(value.isArray())
	{
		std::map<std::string,std::vector<std::string>> map;

		for(auto &it: value)
		{
			if(it.second.isArray())
			{
				for(auto &it2: it.second)
				{
					map[it.first].push_back(it2.second);
				}
			}
			else map[it.first].push_back(it.second);
		}

		_param = map;
	}
};

/**
 * Template for get() method
 *
 * @access public
 * @return T _param
 */
template <typename T>
T Param :: get() {};

/**
 * Template for get() with type as std::string
 *
 * @access public
 * @return <std::string>_param
 * @throw bad type exception
 */
template <>
std::string Param :: get()
{
	if(is_string())
	{
		return boost::get<std::string>(_param);
	}
	else
	{
		return "";
	}
};

/**
 * Template for get() with type as A
 *
 * @access public
 * @return <A>_param
 * @throw bad type exception
 */
template <>
A Param :: get()
{
	if(is_array()) {
		A retval;
		retval = boost::get<A>(_param);
		return retval;
	}
	else
	{
		// Bad type exception
	}
};

/**
 * Template for get() with type as char *
 *
 * @access public
 * @return <char *>_param
 * @throw bad type exception
 */


/**
 * Template for get() with type as const char *
 *
 * @access public
 * @return <const char *>_param
 * @throw bad type exception
 */
template <>
const char * Param :: get()
{
	if(is_string())
	{
		return boost::get<std::string>(_param).c_str();
	}
	else
	{
		//Bad type exception
	}
};
