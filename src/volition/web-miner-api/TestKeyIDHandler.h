// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINERAPI_TESTKEYIDHANDLER_H
#define VOLITION_WEBMINERAPI_TESTKEYIDHANDLER_H

#include <volition/AbstractAPIRequestHandler.h>
#include <volition/Block.h>
#include <volition/CryptoKeyInfo.h>
#include <volition/TheTransactionBodyFactory.h>
#include <volition/TheWebMiner.h>

namespace Volition {
namespace WebMinerAPI {

//================================================================//
// TestKeyIDHandler
//================================================================//
class TestKeyIDHandler :
    public AbstractAPIRequestHandler {
public:

    SUPPORTED_HTTP_METHODS ( HTTP_POST )

    //----------------------------------------------------------------//
    HTTPStatus AbstractAPIRequestHandler_handleRequest ( int method, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {
        UNUSED ( method );
        UNUSED ( jsonOut );

        CryptoKey key;
        FromJSONSerializer::fromJSON ( key, jsonIn );

        CryptoKeyInfo keyInfo ( key, CryptoKeyInfo::HEX );

        if ( keyInfo ) {
            jsonOut.set ( "keyID", key.getKeyID ().c_str ());
            jsonOut.set ( "publicHex", keyInfo.mPublicKey );
            jsonOut.set ( "privateHex", keyInfo.mPrivateKey );
        }
        return Poco::Net::HTTPResponse::HTTP_OK;
    }
};

} // namespace TheWebMinerAPI
} // namespace Volition
#endif
