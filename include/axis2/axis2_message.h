#ifndef AXIS2_MESSAGE_H
#define AXIS2_MESSAGE_H

#include <string>
#include "ws_const.h"
#include "ws_header.h"
#include <boost/variant.hpp>
#include <boost/any.hpp>
#include "param.h"

class Axis2Message {

public:

   /*
    * Attachments
    */
   struct AttachmentsType {
       string contentType     = "application/octet-stream";
       bool useMTOM           = true; //swa
       bool isSWA             = false;
    //   std::map<std::string,char> _map_attachments;
   } attachmentsOpts;


   /**
    * hasMTOM
    */
   bool hasMTOM()
   {
       return attachmentsOpts.useMTOM;
   }

   /**
    * hasSWA
    */
   bool hasSWA()
   {
       return attachmentsOpts.isSWA;
   }

   /**
    * Set Attachments
    */
   void setAttachments(std::map<std::string,char> map)
   {
       if(map.size() > 0)
       {
         //  attachmentsOpts._map_attachments = map;
       }
   }

   /**
    * A URI representing the endpoint URI of the service to be consumed.
    * This must be present on the client side. In case WS-Addressing is in use,
    * this will be used as the WSA To header.
    *
    * Additionaly in case of addressing, you can set reference parameters using the following
    * array of options as following sample code.
    */
   std::shared_ptr<Param> _endpoint{nullptr};

   std::shared_ptr<Param> _reply_to{nullptr};

   std::shared_ptr<Param> _from{nullptr};

   std::shared_ptr<Param> _fault{nullptr};

   std::shared_ptr<Param> _action{nullptr};

   std::shared_ptr<Param> _payload{nullptr};

       /*
     *
     */
    Param * getEndpoint() {
        Param * _p = _endpoint.get();

        return _p;
    }

    /*
     *
     */
    Param * getReplyTo() {
        Param * _p = _reply_to.get();

        return _p;
    }

    /*
     *
     */
    Param * getFrom() {
        Param * _p = _from.get();

        return _p;
    }

    /*
     *
     */
    Param * getFault() {
        Param * _p = _fault.get();

        return _p;
    }

    /**
     *
     */
    Param  * getAction() {
        Param * _p = _action.get();

        return _p;
    }

    bool _wsa_must_understand = false;

    std::string _content_type;

    std::string _rest_content_type;

    std::string _response;

    /**
     * Get WSHeader
     */
    std::vector<WSHeader *> _wsheader_map;

    bool hasAction();

    bool hasPayload();

    bool hasFault();

    bool hasFrom();

    bool hasReply();

    bool hasEndpoint();

    bool hasHeaders();

    template <typename T>
    T get_headers();

    /*
     *
     */
    std::shared_ptr<Param> getPayload();

    Param * getParamByName(const std::string name);

    Axis2Message();

    virtual ~Axis2Message();
};

#endif
