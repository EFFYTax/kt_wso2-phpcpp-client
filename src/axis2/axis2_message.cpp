#include "axis2_message.h"

Axis2Message :: Axis2Message
(){}

Axis2Message :: ~Axis2Message
(){}

/**
 * Proxy to payload
 *
 * @access public
 * @return pointer to <Param>_payload
 */
std::shared_ptr<Param> Axis2Message :: getPayload()
{
    return _payload;
}

/**
 * Proxy to Param object by name
 *
 * TODO: Implement better ( template )
 */
Param * Axis2Message :: getParamByName(const std::string name)
{
    if(name == WSF_REPLY_TO && dynamic_cast<Param *>(_reply_to.get()) != NULL)
    {
        return _reply_to.get();
    }
    else if(name == WSF_FAULT_TO && dynamic_cast<Param *>(_fault.get()) != NULL)
    {
        return _fault.get();
    }
    else if(name == WSF_FROM && dynamic_cast<Param *>(_from.get()) != NULL)
    {
        return _from.get();
    }
    else
    {
        return nullptr;
    }
};

/**
 * Has Message embed headers
 *
 * @access public
 * @return bool
 */
bool Axis2Message :: hasHeaders()
{
    return (!_wsheader_map.empty()) ? true : false;
}

/**
 *
 */
bool Axis2Message :: hasAction()
{
    return (_action) ? true : false;
};

bool Axis2Message :: hasPayload()
{
    return (_payload) ? true : false;
};

bool Axis2Message :: hasFault()
{
    return (_fault) ? true : false;
};

bool Axis2Message :: hasFrom()
{
    return (_from) ? true : false;
};

bool Axis2Message :: hasReply()
{
    return (_reply_to) ? true : false;
};

bool Axis2Message :: hasEndpoint()
{
    return (_endpoint) ? true : false;
};

/**
 * Template get_headers
 */
template <typename T>
T Axis2Message :: get_headers() {};

/**
 * Specialisation for vector
 */
template <>
std::vector<WSHeader *> Axis2Message :: get_headers()
{
    return _wsheader_map;
}



