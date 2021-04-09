// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINERAPI_VISAGEHANDLER_H
#define VOLITION_WEBMINERAPI_VISAGEHANDLER_H

#include <volition/BlockingMinerAPIRequestHandler.h>

namespace Volition {
namespace WebMinerAPI {

//================================================================//
// VisageHandler
//================================================================//
class VisageHandler :
    public BlockingMinerAPIRequestHandler {
public:

    SUPPORTED_HTTP_METHODS ( HTTP::GET )

    //----------------------------------------------------------------//
    HTTPStatus BlockingMinerAPIRequestHandler_handleRequest ( HTTP::Method method, Ledger& ledger, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {
        UNUSED ( method );
        UNUSED ( ledger );
        UNUSED ( jsonIn );
        
        string motto = this->optQuery ( "motto", "" );
                
        Signature visage = this->mWebMiner->calculateVisage ( motto );

        jsonOut.set ( "visage", ToJSONSerializer::toJSON ( visage ));

        return Poco::Net::HTTPResponse::HTTP_OK;
    }
};

} // namespace TheWebMinerAPI
} // namespace Volition
#endif
