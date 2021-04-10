// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINERAPI_DEBUGHTTPECHOHANDLER_H
#define VOLITION_WEBMINERAPI_DEBUGHTTPECHOHANDLER_H

#include <volition/CryptoKey.h>
#include <volition/NonBlockingMinerAPIRequestHandler.h>

namespace Volition {
namespace WebMinerAPI {

//================================================================//
// DebugHTTPEchoHandler
//================================================================//
class DebugHTTPEchoHandler :
    public NonBlockingMinerAPIRequestHandler {
public:

    SUPPORTED_HTTP_METHODS ( HTTP::ALL )

    //----------------------------------------------------------------//
    HTTPStatus NonBlockingMinerAPIRequestHandler_handleRequest ( HTTP::Method method, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {
        
        jsonOut.set ( "method", HTTP::getStringForMethod ( method ).c_str ());
        jsonOut.set ( "body", jsonIn );

        return Poco::Net::HTTPResponse::HTTP_OK;
    }
};

} // namespace TheWebMinerAPI
} // namespace Volition
#endif
