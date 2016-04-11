#ifndef AXIS2_CORE_H
#define AXIS2_CORE_H

#include <phpcpp.h>
#include <axis2_svc_client.h>
#include <axis2_http_transport.h>
#include <axis2_addr.h>
#include <axis2_const.h>
#include <axiom_util.h>
#include <axiom_soap.h>
#include <axutil_env.h>
#include <axis2_const.h>
#include <boost/format.hpp>
#include <string.h>
#include "ws_const.h"


class Axis2Core {

private:

protected:
	axutil_env_t *_env 		  		  = NULL;

	axutil_env_t *_ws_env_svr 		  = NULL;

	std::string _axis2_directory ;

	void set_env();

	int ws_is_svr = 0;

public:

	Axis2Core();

	virtual ~Axis2Core();

	/**
	 * efree wrapper used for Axis2 environemment
	 *
	 * @param  axutil_allocator_t * allocator
	 * @param  void *ptr
	 * @access static
	 * @return void
	 */
	static void free_wrapper_cli(axutil_allocator_t * allocator, void *ptr);

	/**
	 * malloc wrapper used for Axis2 environemment
	 *
	 * @param  axutil_allocator_t * allocator
	 * @param  size_t size
	 * @access static
	 * @return void
	 */
	static void *malloc_wrapper_cli(axutil_allocator_t * allocator, size_t size);

	/**
	 * realloc wrapper used for Axis2 environemment
	 */
	static void *realloc_warpper_cli(axutil_allocator_t * allocator,void *ptr,size_t size);

	/**
	 *
	 */
	bool isFileExist (const std::string& name);

	/**
	 *
	 */
	void log(const std::string v);

	/**
	 *
	 */
	void set_module_param_option (
	    axis2_char_t       * module_name,
	    axis2_char_t       * module_option,
	    axis2_char_t       * module_option_value,
		axis2_svc_client_t * svc_client);

	/**
	 *
	 */
	axis2_char_t * get_axis2_char_ptr(std::string str);
};

#endif
