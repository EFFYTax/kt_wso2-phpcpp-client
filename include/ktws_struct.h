#ifndef KTWS_STRUCT_H
#define KTWS_STRUCT_H

#include <iostream>

/**
 * Structure holding Axis2 Configuration
 */
struct Axis2Config {

    int         log_level;
    std::string log_path;
    std::string log_file;
    std::string home_folder;
};

#endif
