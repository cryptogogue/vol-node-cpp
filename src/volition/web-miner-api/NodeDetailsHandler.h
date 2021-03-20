// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINERAPI_NODEDETAILSHANDLER_H
#define VOLITION_WEBMINERAPI_NODEDETAILSHANDLER_H

#include <volition/NonBlockingMinerAPIRequestHandler.h>

namespace Volition {
namespace WebMinerAPI {

//================================================================//
// NodeDetailsHandler
//================================================================//
class NodeDetailsHandler :
    public NonBlockingMinerAPIRequestHandler {
public:

    SUPPORTED_HTTP_METHODS ( HTTP::GET )

    //----------------------------------------------------------------//
    HTTPStatus NonBlockingMinerAPIRequestHandler_handleRequest ( HTTP::Method method, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {
        UNUSED ( method );
        UNUSED ( jsonIn );
        
        Poco::JSON::Object::Ptr nodeInfoJSON = new Poco::JSON::Object ();
        
        nodeInfoJSON->set ( "minerID",          this->mSnapshot.getMinerID ().c_str ());
        nodeInfoJSON->set ( "publicKey",        ToJSONSerializer::toJSON ( this->mSnapshot.getKeyPair ().getPublicKey ()));
        nodeInfoJSON->set ( "motto",            this->mSnapshot.getMotto ());
        nodeInfoJSON->set ( "visage",           ToJSONSerializer::toJSON ( this->mSnapshot.getVisage ()));
        
        jsonOut.set ( "node", nodeInfoJSON );
        return Poco::Net::HTTPResponse::HTTP_OK;
    }
};

} // namespace TheWebMinerAPI
} // namespace Volition
#endif
