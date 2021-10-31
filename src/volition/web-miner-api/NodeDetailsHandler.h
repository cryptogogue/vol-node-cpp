// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINERAPI_NODEDETAILSHANDLER_H
#define VOLITION_WEBMINERAPI_NODEDETAILSHANDLER_H

#include <volition/AbstractMinerAPIRequestHandler.h>

namespace Volition {
namespace WebMinerAPI {

//================================================================//
// NodeDetailsHandler
//================================================================//
class NodeDetailsHandler :
    public AbstractMinerAPIRequestHandler {
public:

    SUPPORTED_HTTP_METHODS ( HTTP::GET )

    //----------------------------------------------------------------//
    HTTPStatus AbstractMinerAPIRequestHandler_handleRequest ( HTTP::Method method, shared_ptr < Miner > miner, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {
        UNUSED ( method );
        UNUSED ( jsonIn );
        
        ScopedSharedMinerStatusLock minerStatus ( miner );
        
        Poco::JSON::Object::Ptr nodeInfoJSON = new Poco::JSON::Object ();
        
        nodeInfoJSON->set ( "minerID",          minerStatus.getMinerID ().c_str ());
        nodeInfoJSON->set ( "publicKey",        ToJSONSerializer::toJSON ( minerStatus.getKeyPair ().getPublicKey ()));
        nodeInfoJSON->set ( "motto",            minerStatus.getMotto ());
        nodeInfoJSON->set ( "visage",           ToJSONSerializer::toJSON ( minerStatus.getVisage ()));
        nodeInfoJSON->set ( "genesisHash",      minerStatus.mGenesisHash );
        nodeInfoJSON->set ( "acceptedRelease",  minerStatus.mAcceptedRelease ); // the release we're actually ready to accept
        nodeInfoJSON->set ( "nextRelease",      VOL_NODE_RELEASE ); // always striving for the current build's release
        
        jsonOut.set ( "node", nodeInfoJSON );
        return Poco::Net::HTTPResponse::HTTP_OK;
    }
};

} // namespace TheWebMinerAPI
} // namespace Volition
#endif
