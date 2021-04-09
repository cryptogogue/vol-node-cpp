// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINERAPI_ACCOUNTDETAILSHANDLER_H
#define VOLITION_WEBMINERAPI_ACCOUNTDETAILSHANDLER_H

#include <volition/Block.h>
#include <volition/SemiBlockingMinerAPIRequestHandler.h>
#include <volition/TheTransactionBodyFactory.h>
#include <volition/web-miner-api/AccountDetailsHandler.h>

namespace Volition {
namespace WebMinerAPI {

//================================================================//
// KeyAccountDetailsHandler
//================================================================//
class KeyAccountDetailsHandler :
    public SemiBlockingMinerAPIRequestHandler {
public:

    SUPPORTED_HTTP_METHODS ( HTTP::GET )

    //----------------------------------------------------------------//
    HTTPStatus SemiBlockingMinerAPIRequestHandler_handleRequest ( HTTP::Method method, AbstractLedger& ledger, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {
        UNUSED ( method );
        UNUSED ( jsonIn );
    
        string keyID = this->getMatchString ( "keyHash" );

        shared_ptr < AccountKeyLookup > accountKeyLookup = ledger.getAccountKeyLookup ( keyID );
        if ( accountKeyLookup ) {
        
            AccountODBM accountODBM ( ledger, accountKeyLookup->mAccountIndex );
            if ( accountODBM ) {
                AccountDetailsHandler::formatJSON ( ledger, accountODBM, jsonOut );
                return Poco::Net::HTTPResponse::HTTP_OK;
            }
        }
        return Poco::Net::HTTPResponse::HTTP_NOT_FOUND;
    }
};

} // namespace TheWebMinerAPI
} // namespace Volition
#endif
