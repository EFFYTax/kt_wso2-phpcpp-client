#ifndef KTWS_WSDL_PHP_H
#define KTWS_WSDL_PHP_H

#include <phpcpp.h>
#include <iostream>

using namespace std;

/*
 * Class interacting with WSDL
 */
class Wsdl :public Php::Base {

public:
    /*
     * c++ ctor
     */
    Wsdl();

    /*
     * c++ dtor
     */
    ~Wsdl();

    /*
     * php __ctor
     */
    void __construct();

    /*
     *
     */
    void setParameter(Php::Parameters &params);

    /**
     *
     */
    Php::Value getDebug();
};

#endif
