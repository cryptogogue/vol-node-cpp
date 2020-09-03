// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINERAPI_TESTSIGNATUREHANDLER_H
#define VOLITION_WEBMINERAPI_TESTSIGNATUREHANDLER_H

#include <volition/Block.h>
#include <volition/AbstractAPIRequestHandler.h>
#include <volition/TheTransactionBodyFactory.h>
#include <volition/WebMinerAPIFactory.h>

namespace Volition {
namespace WebMinerAPI {

//================================================================//
// TestSignatureHandler
//================================================================//
class TestSignatureHandler :
    public WebMinerAPIRequestHandler {
public:

    SUPPORTED_HTTP_METHODS ( HTTP::POST )

    //----------------------------------------------------------------//
    HTTPStatus AbstractAPIRequestHandler_handleRequest ( HTTP::Method method, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {
        UNUSED ( method );
        UNUSED ( jsonOut );

        const Poco::JSON::Object::Ptr keyJSON = jsonIn.getObject ( "key" );
        const Poco::JSON::Object::Ptr sigJSON = jsonIn.getObject ( "signature" );

        if ( keyJSON && sigJSON ) {
            
            string message = jsonIn.optValue < string >( "message", "" );

            CryptoKey key;
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
