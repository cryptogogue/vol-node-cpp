// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINERAPI_ACCOUNTKEYLISTHANDLER_H
#define VOLITION_WEBMINERAPI_ACCOUNTKEYLISTHANDLER_H

#include <volition/AbstractMinerAPIRequestHandler.h>
#include <volition/Block.h>
#include <volition/MinerAPIFactory.h>
#include <volition/TheTransactionBodyFactory.h>

namespace Volition {
namespace WebMinerAPI {

//================================================================//
// AccountKeyListHandler
//================================================================//
class AccountKeyListHandler :
    public AbstractMinerAPIRequestHandler {
public:

    SUPPORTED_HTTP_METHODS ( HTTP::GET )

    //----------------------------------------------------------------//
    HTTPStatus AbstractMinerAPIRequestHandler_handleRequest ( HTTP::Method method, Ledger& ledger, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {
        UNUSED ( method );
        UNUSED ( jsonIn );
    
        string accountName = this->getMatchString ( "accountName" );

        shared_ptr < const Account > account = AccountODBM ( ledger, accountName ).mBody.get ();
        if ( !account ) return Poco::Net::HTTPResponse::HTTP_NOT_FOUND;

        SerializableMap < string, KeyAndPolicy > keys;
        account->getKeys ( keys );
        jsonOut.set ( "accountKeys", ToJSONSerializer::toJSON ( keys ));
        return Poco::Net::HTTPResponse::HTTP_OK;
    }
};

} // namespace TheWebMinerAPI
} // namespace Volition
#endif
