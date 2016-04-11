#include <axis2_core.h>
#include <iostream>
#include <sys/stat.h>
#include <unistd.h>

/**
 * c++ ctor
 */
Axis2Core :: Axis2Core() {

	_axis2_directory = Php::ini_get("ktwso2.axis2").stringValue();

	set_env();
};

/**
 * c++ dtor
 */
Axis2Core :: ~Axis2Core()
{
	axutil_env_free(_env);
};

/**
 * Make a working environement for Axis2
 */
void Axis2Core :: set_env() {
	axutil_allocator_t   *allocator 	= nullptr;
	axutil_error_t       *error 		= nullptr;
	axutil_log_t         *log 			= nullptr;
	axis2_char_t         log_path[250];
	axutil_thread_pool_t *thread_pool 	= nullptr;
	const axis2_char_t   *log_name 		= "wsf_php_client.log";
	const axis2_char_t   *path_tolog 	= "/tmp";

	allocator = (axutil_allocator_t *) malloc(sizeof(axutil_allocator_t));

	allocator->free_fn 		= free_wrapper_cli;
	allocator->malloc_fn 	= malloc_wrapper_cli;
	allocator->realloc 		= realloc_warpper_cli;
	allocator->local_pool 	= nullptr;
	allocator->global_pool  = nullptr;

	//{{{ Alexis Gruet
	//Not available since we use axis2c-unofficial!
	//allocator->global_pool_ref 	= 0;

	error = axutil_error_create(allocator);

	if (path_tolog
			&& ((0 == strcmp(path_tolog, "")) || (0 == strcmp(path_tolog, "."))
					|| (0 == strcmp(path_tolog, "./")))) {
		snprintf(log_path, 256, "%s", log_name);
	} else {
		snprintf(log_path, 256, "%s/%s", path_tolog, log_name);
	}

	thread_pool = axutil_thread_pool_init(allocator);
	log         = axutil_log_create(allocator, nullptr, log_path);

	_env = axutil_env_create_with_error_log_thread_pool(allocator, error, log, thread_pool);
};

/**
 * efree wrapper
 */
void Axis2Core :: free_wrapper_cli(axutil_allocator_t * allocator, void *ptr) {
	if (ptr) {
		free(ptr);
	}
};

/**
 * malloc wrapper
 */
void *Axis2Core :: malloc_wrapper_cli(axutil_allocator_t * allocator,
		size_t size) {
	return malloc(size);
};

/**
 * realloc wrapper
 */
void *Axis2Core :: realloc_warpper_cli(axutil_allocator_t * allocator, void *ptr,
		size_t size) {
	return realloc(ptr, size);
};

/**
 * is file exist
 */
bool Axis2Core :: isFileExist (const std::string& name) {
  struct stat buffer;
  return (stat (name.c_str(), &buffer) == 0);
};


/**
 *
 */
void Axis2Core :: log(const std::string v) {
	AXIS2_LOG_DEBUG (_env->log, AXIS2_LOG_SI, std::string(WSF_PHP_LOG_PREFIX + v).c_str());
}

/**
 * Set parameters for a given Axis2c module
 *
 * @access private
 *
 * @param module_name, the module name ( i.e sandesha2 )
 * @param module_option, the module option to modify ( key )
 * @param module_option_value, the module option value ( val )
 *
 * Return AXIS2_SUCESS | AXIS2_FAILLURE
 */
void Axis2Core :: set_module_param_option (
    axis2_char_t       * module_name,
    axis2_char_t       * module_option,
    axis2_char_t       * module_option_value,
	axis2_svc_client_t * svc_client) {

	axis2_conf_ctx_t    * conf_ctx 		 = nullptr;
	axis2_svc_ctx_t     * svc_ctx 		 = nullptr;
	axis2_module_desc_t * module_desc    = nullptr;
	axis2_conf_t        * conf 		     = nullptr;
	axutil_qname_t      * module_qname 	 = nullptr;
	axutil_param_t      * param 		 = nullptr;

	module_qname = axutil_qname_create          (_env, module_name, nullptr, nullptr);
	svc_ctx 	 = axis2_svc_client_get_svc_ctx (svc_client, _env);
	conf_ctx 	 = axis2_svc_ctx_get_conf_ctx   (svc_ctx, _env);
	conf 		 = axis2_conf_ctx_get_conf      (conf_ctx, _env);
	module_desc  = axis2_conf_get_module        (conf, _env, module_qname);

	if (!module_desc)
	{
		throw Php::Exception("Issue in set module param option (module_desc)");
		log("module could not be configured");
	}
	param = axis2_module_desc_get_param (module_desc, _env, module_option);

	if (!param)
	{
		throw Php::Exception("Issue in set module param option (param)");
		log("module could not be configured");
	}
	axutil_param_set_value (param, _env, axutil_strdup (_env, module_option_value));

	log((boost::format("Setting %1% module param %2% to %3%")
		% module_name
		% module_option
		% module_option_value).str());

	axutil_qname_free (module_qname, _env);
};

/**
 *
 */
axis2_char_t * Axis2Core :: get_axis2_char_ptr(std::string str)
{
	axis2_char_t * ptr = strdup(str.c_str());
	return ptr;
};

