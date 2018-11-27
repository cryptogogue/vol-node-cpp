// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINERAPI_ACCOUNTKEYLISTHANDLER_H
#define VOLITION_WEBMINERAPI_ACCOUNTKEYLISTHANDLER_H

#include <volition/Block.h>
#include <volition/AbstractAPIRequestHandler.h>
#include <volition/TheTransactionFactory.h>
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
    
//        string accountName = this->getMatchString ( "accountName" );
//        const state& ledger = TheWebMiner::get ().getLedger ();
//
//        VersionedValue < Account > account = ledger.getAccount ( accountName );
//        if ( account ) {
//
//            map < string, CryptoKey > keys;
//            account->getKeys ( keys );
//
//            Poco::JSON::Object::Ptr keysJSON = new Poco::JSON::Object ();
//
//            map < string, CryptoKey >::iterator keyIt = keys.begin ();
//            for ( ; keyIt != keys.end (); ++keyIt ) {
//                Poco::JSON::Object::Ptr keyJSON = ToJSONSerializer::toJSON ( keyIt->second );
//                keysJSON->set ( keyIt->first, keyJSON );
//            }
//
//            jsonOut.set ( "accountKeys", keysJSON );
//            return Poco::Net::HTTPResponse::HTTP_OK;
//        }
        return Poco::Net::HTTPResponse::HTTP_NOT_FOUND;
    }
};

} // namespace TheWebMinerAPI
} // namespace Volition
#endif
