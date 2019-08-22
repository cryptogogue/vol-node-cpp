// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINERAPI_ACCOUNTKEYLISTHANDLER_H
#define VOLITION_WEBMINERAPI_ACCOUNTKEYLISTHANDLER_H

#include <volition/Block.h>
#include <volition/AbstractAPIRequestHandler.h>
#include <volition/TheTransactionBodyFactory.h>
#include <volition/TheWebMiner.h>

namespace Volition {
namespace WebMinerAPI {

//================================================================//
// AccountKeyListHandler
//================================================================//
class AccountKeyListHandler :
    public AbstractAPIRequestHandler {
public:

    SUPPORTED_HTTP_METHODS ( HTTP_GET )

    //----------------------------------------------------------------//
    HTTPStatus AbstractAPIRequestHandler_handleRequest ( int method, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {
        UNUSED ( method );
        UNUSED ( jsonIn );
    
        string accountName = this->getMatchString ( "accountName" );

        ScopedWebMinerLock scopedLock ( TheWebMiner::get ());
        const Chain& chain = *scopedLock.getWebMiner ().getBestBranch ();

        shared_ptr < Account > account = chain.getAccount ( accountName );
        if ( account ) {

            SerializableMap < string, CryptoKey > keys;
            account->getKeys ( keys );
            jsonOut.set ( "accountKeys", ToJSONSerializer::toJSON ( keys ));
            return Poco::Net::HTTPResponse::HTTP_OK;
        }
        return Poco::Net::HTTPResponse::HTTP_NOT_FOUND;
    }
};

} // namespace TheWebMinerAPI
} // namespace Volition
#endif
