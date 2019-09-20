// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_ABSTRACTAPIREQUESTHANDLER_H
#define VOLITION_ABSTRACTAPIREQUESTHANDLER_H

#include <volition/AbstractRequestHandler.h>
#include <volition/HTTP.h>

namespace Volition {

#define SUPPORTED_HTTP_METHODS(mask)                                                        \
    HTTP::Method AbstractAPIRequestHandler_getSupportedHTTPMethods () const override {      \
        return mask;                                                                        \
    }

//================================================================//
// AbstractAPIRequestHandler
//================================================================//
class AbstractAPIRequestHandler :
    public AbstractRequestHandler {
protected:
    
    typedef Poco::Net::HTTPResponse::HTTPStatus HTTPStatus;
    
    //----------------------------------------------------------------//
    void                    AbstractRequestHandler_handleRequest                    ( const Routing::PathMatch& match, Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response ) const override;

    //----------------------------------------------------------------//
    virtual HTTP::Method    AbstractAPIRequestHandler_getSupportedHTTPMethods       () const;
    virtual HTTPStatus      AbstractAPIRequestHandler_handleDelete                  () const;
    virtual HTTPStatus      AbstractAPIRequestHandler_handleGet                     ( Poco::JSON::Object& jsonOut ) const;
    virtual HTTPStatus      AbstractAPIRequestHandler_handlePatch                   ( const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const;
    virtual HTTPStatus      AbstractAPIRequestHandler_handlePost                    ( const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const;
    virtual HTTPStatus      AbstractAPIRequestHandler_handlePut                     ( const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const;
    virtual HTTPStatus      AbstractAPIRequestHandler_handleRequest                 ( HTTP::Method method, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const;

public:

    //----------------------------------------------------------------//
                            AbstractAPIRequestHandler                   ();
                            ~AbstractAPIRequestHandler                  ();
};

} // namespace Volition
#endif
