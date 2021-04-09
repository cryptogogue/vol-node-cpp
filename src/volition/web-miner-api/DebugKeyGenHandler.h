// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINERAPI_DEBUGKEYGENHANDLER_H
#define VOLITION_WEBMINERAPI_DEBUGKEYGENHANDLER_H

#include <volition/CryptoKey.h>
#include <volition/SemiBlockingMinerAPIRequestHandler.h>

namespace Volition {
namespace WebMinerAPI {

//================================================================//
// DebugKeyGenHandler
//================================================================//
class DebugKeyGenHandler :
    public SemiBlockingMinerAPIRequestHandler {
public:

    SUPPORTED_HTTP_METHODS ( HTTP::GET )

    //----------------------------------------------------------------//
    HTTPStatus SemiBlockingMinerAPIRequestHandler_handleRequest ( HTTP::Method method, AbstractLedger& ledger, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {
        UNUSED ( method );
        UNUSED ( ledger );
        UNUSED ( jsonIn );
        
        string type = this->getMatchString ( "type" );

        if ( type == "ec" ) {
        
            CryptoKeyPair keyPair;
            keyPair.elliptic ();
            string pem = keyPair.toPEM ();
            printf ( "%s\n", pem.c_str ());
            jsonOut.set ( "pem", pem );
        }

        return Poco::Net::HTTPResponse::HTTP_OK;
    }
};

} // namespace TheWebMinerAPI
} // namespace Volition
#endif
