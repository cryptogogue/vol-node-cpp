// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINERAPI_NODEDETAILSHANDLER_H
#define VOLITION_WEBMINERAPI_NODEDETAILSHANDLER_H

#include <volition/AccountODBM.h>
#include <volition/AbstractAPIRequestHandler.h>
#include <volition/Block.h>
#include <volition/TheTransactionBodyFactory.h>
#include <volition/MinerAPIFactory.h>

namespace Volition {
namespace WebMinerAPI {

//================================================================//
// NodeDetailsHandler
//================================================================//
class NodeDetailsHandler :
    public MinerAPIRequestHandler {
public:

    SUPPORTED_HTTP_METHODS ( HTTP::GET )

    //----------------------------------------------------------------//
    HTTPStatus AbstractAPIRequestHandler_handleRequest ( HTTP::Method method, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {
        UNUSED ( method );
        UNUSED ( jsonIn );
    
        ScopedMinerLock scopedLock ( this->mWebMiner );
        
        Poco::JSON::Object::Ptr nodeInfoJSON = new Poco::JSON::Object ();
        
        nodeInfoJSON->set ( "publicKey",        ToJSONSerializer::toJSON ( this->mWebMiner->getKeyPair ().getPublicKey ()));
        nodeInfoJSON->set ( "motto",            this->mWebMiner->getMotto ());
        nodeInfoJSON->set ( "visage",           ToJSONSerializer::toJSON ( this->mWebMiner->getVisage ()));
        
        jsonOut.set ( "node", nodeInfoJSON );
        return Poco::Net::HTTPResponse::HTTP_OK;
    }
};

} // namespace TheWebMinerAPI
} // namespace Volition
#endif
