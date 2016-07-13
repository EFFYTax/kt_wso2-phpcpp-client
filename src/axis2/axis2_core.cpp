#include <axis2_core.h>
#include <iostream>
#include <sys/stat.h>
#include <unistd.h>

/**
 * c++ ctor
 */
Axis2Core :: Axis2Core(const Axis2Config& config) : a2config(config) {

   	//allocation / axis2/c env
    initEnvironment();

    if(!env || !env->log)
        throw std::invalid_argument("Issue while initializing Axis2/c environement");

	env->log->level = [&] () -> axutil_log_levels_t  {
		axutil_log_levels_t log_level = (axutil_log_levels_t) a2config.log_level;
		if(log_level < 0 || log_level > 6) log_level = AXIS2_LOG_LEVEL_DEBUG;
		return log_level;
	}();
};

/**
 * c++ dtor
 */
Axis2Core :: ~Axis2Core()
{
	axutil_env_free(env.get());
};

/**
 * Make a working environement for Axis2
 */
void Axis2Core :: initEnvironment() {

   	allocator.reset(axutil_allocator_init(NULL));

	error.reset(axutil_error_create(allocator.get()));

	std::string full_path = "";
	full_path
	.append(a2config.log_path)
	.append("/")
	.append(a2config.log_file);

	thread_pool.reset(axutil_thread_pool_init(allocator.get()));

	_log.reset(axutil_log_create(allocator.get(),
	        nullptr, full_path.c_str()));

	env.reset(axutil_env_create_with_error_log_thread_pool(allocator.get(),
	        error.get(), _log.get(), thread_pool.get()));

	if(!env) throw std::runtime_error("Unable to create axis2/c env");
};

/**
 * is file exist
 */
bool Axis2Core :: isFileExist (const std::string& name) {
  struct stat buffer;
  return (stat (name.c_str(), &buffer) == 0);
};

/**
 * Logger proxy
 */
void Axis2Core :: log(const std::string v, const std::string file, int line) {
	axutil_log_impl_log_debug(env->log, file.c_str(), line, "%s %s", AXIS2_LOG_PROJECT_PREFIX, v.c_str());
}
