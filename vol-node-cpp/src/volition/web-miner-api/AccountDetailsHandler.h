// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINERAPI_KEYACCOUNTDETAILSHANDLER_H
#define VOLITION_WEBMINERAPI_KEYACCOUNTDETAILSHANDLER_H

#include <volition/AccountODBM.h>
#include <volition/Block.h>
#include <volition/AbstractAPIRequestHandler.h>
#include <volition/TheTransactionBodyFactory.h>
#include <volition/MinerAPIFactory.h>

namespace Volition {
namespace WebMinerAPI {

//================================================================//
// AccountDetailsHandler
//================================================================//
class AccountDetailsHandler :
    public MinerAPIRequestHandler {
public:

    SUPPORTED_HTTP_METHODS ( HTTP::GET )

    //----------------------------------------------------------------//
    static void formatJSON ( const Ledger& ledger, const Account& account, Poco::JSON::Object& jsonOut ) {
    
        AccountODBM accountODBM ( ledger, account.mIndex );
        
        // account's "primary" name
        string accountName = ledger.getAccountName ( account.mIndex );
        
        // get the account JSON
        Poco::JSON::Object::Ptr accountJSON = ToJSONSerializer::toJSON ( account ).extract < Poco::JSON::Object::Ptr >();
        
        // decorate with virtual fields
        accountJSON->set ( "name", accountName );
        accountJSON->set ( "assetCount", accountODBM.mAssetCount.get ( 0 ));
        accountJSON->set ( "inventoryNonce", accountODBM.mInventoryNonce.get ( 0 ));
        accountJSON->set ( "nonce", accountODBM.mTransactionNonce.get ( 0 ));
        accountJSON->set ( "height", ledger.getHeight ());
        
        jsonOut.set ( "account", accountJSON );
        
        ToJSONSerializer entitlements;
        ledger.serializeEntitlements ( account, entitlements );
        jsonOut.set ( "entitlements", entitlements );
    }

    //----------------------------------------------------------------//
    HTTPStatus AbstractAPIRequestHandler_handleRequest ( HTTP::Method method, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {
        UNUSED ( method );
        UNUSED ( jsonIn );
    
        string accountName = this->getMatchString ( "accountName" );
        
        ScopedMinerLock scopedLock ( this->mWebMiner );
        const Ledger& ledger = this->mWebMiner->getLedger ();

        shared_ptr < Account > account = ledger.getAccount ( ledger.getAccountIndex ( accountName ));
        if ( account ) {
            AccountDetailsHandler::formatJSON ( ledger, *account, jsonOut );
            return Poco::Net::HTTPResponse::HTTP_OK;
        }
        return Poco::Net::HTTPResponse::HTTP_NOT_FOUND;
    }
};

} // namespace TheWebMinerAPI
} // namespace Volition
#endif
