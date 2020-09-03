// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINERAPI_ACCOUNTDETAILSHANDLER_H
#define VOLITION_WEBMINERAPI_ACCOUNTDETAILSHANDLER_H

#include <volition/Block.h>
#include <volition/AbstractAPIRequestHandler.h>
#include <volition/TheTransactionBodyFactory.h>
#include <volition/web-miner-api/AccountDetailsHandler.h>
#include <volition/WebMinerAPIFactory.h>

namespace Volition {
namespace WebMinerAPI {

//================================================================//
// KeyAccountDetailsHandler
//================================================================//
class KeyAccountDetailsHandler :
    public WebMinerAPIRequestHandler {
public:

    SUPPORTED_HTTP_METHODS ( HTTP::GET )

    //----------------------------------------------------------------//
    HTTPStatus AbstractAPIRequestHandler_handleRequest ( HTTP::Method method, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {
        UNUSED ( method );
        UNUSED ( jsonIn );
    
        string keyID = this->getMatchString ( "keyHash" );
        
        ScopedWebMinerLock scopedLock ( this->mWebMiner );
        const Ledger& ledger = this->mWebMiner->getLedger ();

        shared_ptr < AccountKeyLookup > accountKeyLookup = ledger.getAccountKeyLookup ( keyID );
        if ( accountKeyLookup ) {
            shared_ptr < Account > account = ledger.getAccount ( accountKeyLookup->mAccountIndex );
            if ( account ) {
                AccountDetailsHandler::formatJSON ( ledger, *account, jsonOut );
                return Poco::Net::HTTPResponse::HTTP_OK;
            }
        }
        return Poco::Net::HTTPResponse::HTTP_NOT_FOUND;
    }
};

} // namespace TheWebMinerAPI
} // namespace Volition
#endif
