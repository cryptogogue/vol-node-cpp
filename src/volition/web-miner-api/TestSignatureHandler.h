// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINERAPI_TESTSIGNATUREHANDLER_H
#define VOLITION_WEBMINERAPI_TESTSIGNATUREHANDLER_H

#include <volition/Block.h>
#include <volition/AbstractMinerAPIRequestHandler.h>
#include <volition/TheTransactionBodyFactory.h>

namespace Volition {
namespace WebMinerAPI {

//================================================================//
// TestSignatureHandler
//================================================================//
class TestSignatureHandler :
    public AbstractMinerAPIRequestHandler {
public:

    SUPPORTED_HTTP_METHODS ( HTTP::POST )

    //----------------------------------------------------------------//
    HTTPStatus AbstractMinerAPIRequestHandler_handleRequest ( HTTP::Method method, shared_ptr < Miner > miner, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {
        UNUSED ( method );
        UNUSED ( miner ) ;
        UNUSED ( jsonOut );

        const Poco::JSON::Object::Ptr keyJSON = jsonIn.getObject ( "key" );
        const Poco::JSON::Object::Ptr sigJSON = jsonIn.getObject ( "signature" );

        if ( keyJSON && sigJSON ) {
            
            string message = jsonIn.optValue < string >( "message", "" );

            CryptoKeyPair key;
            FromJSONSerializer::fromJSON ( key, keyJSON );

            Signature signature;
            FromJSONSerializer::fromJSON ( signature, sigJSON );

            if ( key && signature ) {
            
                bool isOK = key.verify ( signature, message );
                jsonOut.set ( "OK", isOK ? "true" : "false" );
            }
        }
        return Poco::Net::HTTPResponse::HTTP_OK;
    }
};

} // namespace TheWebMinerAPI
} // namespace Volition
#endif
