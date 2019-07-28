// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINERAPI_TESTKEYIDHANDLER_H
#define VOLITION_WEBMINERAPI_TESTKEYIDHANDLER_H

#include <volition/Block.h>
#include <volition/AbstractAPIRequestHandler.h>
#include <volition/TheTransactionFactory.h>
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

        CryptoKeyInfo keyInfo = key.getInfo ();

        if ( keyInfo.mIsValid ) {
            jsonOut.set ( "keyID", key.getKeyID ().c_str ());
            jsonOut.set ( "publicHex", keyInfo.mPubKeyHex );
            jsonOut.set ( "privateHex", keyInfo.mPrivKeyHex );
        }
        return Poco::Net::HTTPResponse::HTTP_OK;
    }
};

} // namespace TheWebMinerAPI
} // namespace Volition
#endif
