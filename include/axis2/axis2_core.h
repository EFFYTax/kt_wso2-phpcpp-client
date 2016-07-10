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
#include <ktws_struct.h>

class Axis2Core {

public:

    Axis2Config a2config;

    /*
     * Axis2 Memory allocator, we use the default implementation ( @see axutil_allocator_free_impl )
     */
    std::unique_ptr<axutil_allocator_t, std::function<void(axutil_allocator_t *)>> allocator{nullptr,
        [&](axutil_allocator_t * p){ axutil_env_free(env.get()); }
    };

    /*
     * Axis2 Error, Valgrind reported that it will be free'd by axutil_allocator_free_impl
     */
    std::unique_ptr<axutil_error_t, std::function<void(axutil_error_t *)>> error{nullptr,
        [](axutil_error_t * p){}
    };

    /*
     * Axis2 Log, Valgrind reported that it will be free'd by axutil_allocator_free_impl
     */
    std::unique_ptr<axutil_log_t, std::function<void(axutil_log_t *)>> _log{nullptr,
        [](axutil_log_t * p){}
    };

    /*
     * Axis2 Thread pool, Valgrind reported that it will be free'd by axutil_allocator_free_impl
     */
    std::unique_ptr<axutil_thread_pool_t, std::function<void(axutil_thread_pool_t *)>> thread_pool{nullptr,
        [](axutil_thread_pool_t * p){}
    };

    /*
     * Init the Axis2 environement
     *
     * @return void
     */
    void initEnvironment();

    /*
     * Axis2 environement, Valgrind reported that it will be free'd by axutil_allocator_free_impl
     */
    std::unique_ptr<axutil_env_t, std::function<void (axutil_env_t *)>> env{nullptr,
        [&](axutil_env_t *p ){}
    };

	/**
	 * TODO: Implement
	 */
	int ws_is_svr = 0;

	/**
	 * c++ ctor
	 */
	Axis2Core(const Axis2Config& configuration);

	/**
	 * c++ dtor
	 */
	virtual ~Axis2Core();

	/**
	 * Is a file Exist ?
	 */
	bool isFileExist (const std::string& name);

	/**
	 *
	 */
	void log(const std::string v, const std::string file, int line);
};

#endif
