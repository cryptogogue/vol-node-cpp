// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_ABSTRACTAPIREQUESTHANDLER_H
#define VOLITION_ABSTRACTAPIREQUESTHANDLER_H

#include <volition/AbstractRequestHandler.h>

namespace Volition {

#define SUPPORTED_HTTP_METHODS(mask)                                                \
    int AbstractAPIRequestHandler_getSupportedHTTPMethods () const override {       \
        return mask;                                                                \
    }

//================================================================//
// AbstractAPIRequestHandler
//================================================================//
class AbstractAPIRequestHandler :
    public AbstractRequestHandler {
protected:
    
    typedef Poco::Net::HTTPResponse::HTTPStatus HTTPStatus;

    static const int HTTP_ALL           = -1;
    static const int HTTP_DELETE        =  1 << 0;
    static const int HTTP_GET           =  1 << 1;
    static const int HTTP_HEAD          =  1 << 2;
    static const int HTTP_PATCH         =  1 << 3;
    static const int HTTP_POST          =  1 << 4;
    static const int HTTP_PUT           =  1 << 5;
    
    //----------------------------------------------------------------//
    static int              getMethodForString                          ( string method );
    void                    AbstractRequestHandler_handleRequest        ( const Routing::PathMatch& match, Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response ) const override;

    //----------------------------------------------------------------//
    virtual int             AbstractAPIRequestHandler_getSupportedHTTPMethods       () const = 0;
    virtual HTTPStatus      AbstractAPIRequestHandler_handleRequest                 ( int method, const Routing::PathMatch& match, const Poco::JSON::Object::Ptr jsonIn, Poco::JSON::Object::Ptr& jsonOut ) const = 0;

public:

    //----------------------------------------------------------------//
                            AbstractAPIRequestHandler                   ();
                            ~AbstractAPIRequestHandler                  ();
};

} // namespace Volition
#endif
