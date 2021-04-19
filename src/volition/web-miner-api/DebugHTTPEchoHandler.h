// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINERAPI_DEBUGHTTPECHOHANDLER_H
#define VOLITION_WEBMINERAPI_DEBUGHTTPECHOHANDLER_H

#include <volition/CryptoKey.h>
#include <volition/AbstractMinerAPIRequestHandler.h>

namespace Volition {
namespace WebMinerAPI {

//================================================================//
// DebugHTTPEchoHandler
//================================================================//
class DebugHTTPEchoHandler :
    public AbstractMinerAPIRequestHandler {
public:

    SUPPORTED_HTTP_METHODS ( HTTP::ALL )

    //----------------------------------------------------------------//
    HTTPStatus AbstractMinerAPIRequestHandler_handleRequest ( HTTP::Method method, shared_ptr < Miner > miner, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {
        UNUSED ( miner );
        
        jsonOut.set ( "method", HTTP::getStringForMethod ( method ).c_str ());
        jsonOut.set ( "body", jsonIn );

        return Poco::Net::HTTPResponse::HTTP_OK;
    }
};

} // namespace TheWebMinerAPI
} // namespace Volition
#endif
