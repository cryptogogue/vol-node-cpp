// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINERAPI_TESTKEYIDHANDLER_H
#define VOLITION_WEBMINERAPI_TESTKEYIDHANDLER_H

#include <volition/AbstractAPIRequestHandler.h>
#include <volition/Block.h>
#include <volition/CryptoKeyInfo.h>
#include <volition/TheTransactionBodyFactory.h>
#include <volition/MinerAPIFactory.h>

namespace Volition {
namespace WebMinerAPI {

//================================================================//
// TestKeyIDHandler
//================================================================//
class TestKeyIDHandler :
    public MinerAPIRequestHandler {
public:

    SUPPORTED_HTTP_METHODS ( HTTP::POST )

    //----------------------------------------------------------------//
    HTTPStatus AbstractAPIRequestHandler_handleRequest ( HTTP::Method method, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {
        UNUSED ( method );
        UNUSED ( jsonOut );

        CryptoKeyPair key;
        FromJSONSerializer::fromJSON ( key, jsonIn );

        CryptoKeyInfo keyInfo ( key.getKeyPair (), CryptoKeyInfo::ENCODE_AS_HEX );

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
