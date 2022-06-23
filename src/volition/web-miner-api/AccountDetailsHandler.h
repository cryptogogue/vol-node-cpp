// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINERAPI_KEYACCOUNTDETAILSHANDLER_H
#define VOLITION_WEBMINERAPI_KEYACCOUNTDETAILSHANDLER_H

#include <volition/AccountODBM.h>
#include <volition/AssetODBM.h>
#include <volition/Block.h>
#include <volition/AbstractMinerAPIRequestHandler.h>
#include <volition/TheTransactionBodyFactory.h>

namespace Volition {
namespace WebMinerAPI {

//================================================================//
// AccountDetailsHandler
//================================================================//
class AccountDetailsHandler :
    public AbstractMinerAPIRequestHandler {
public:

    SUPPORTED_HTTP_METHODS ( HTTP::GET )

    //----------------------------------------------------------------//
    static void formatJSON ( const AbstractLedger& ledger, AccountODBM& accountODBM, Poco::JSON::Object& jsonOut ) {
                
        // account's "primary" name
        string accountName = accountODBM.mName.get ();
        
        shared_ptr < const Account > account = accountODBM.mBody.get ();
        
        // get the account JSON
        Poco::JSON::Object::Ptr accountJSON = ToJSONSerializer::toJSON ( *account ).extract < Poco::JSON::Object::Ptr >();
        
        // decorate with virtual fields
        accountJSON->set ( "name",                  accountName );
        accountJSON->set ( "index",                 ( u64 )accountODBM.mAccountID );
        accountJSON->set ( "assetCount",            accountODBM.mAssetCount.get ( 0 ));
        accountJSON->set ( "fingerprint",           accountODBM.mFingerprint.get ( "" ));
        accountJSON->set ( "identityProviderName",    accountODBM.mIdentityProviderName.get ( "" ));
        accountJSON->set ( "inventoryNonce",        accountODBM.mInventoryNonce.get ( 0 ));
        accountJSON->set ( "nonce",                 accountODBM.mTransactionNonce.get ( 0 ));
        accountJSON->set ( "height",                ledger.getHeight ());
        accountJSON->set ( "balance",               accountODBM.mBalance.get ( 0 ));
        
        jsonOut.set ( "account", accountJSON );
        
        ToJSONSerializer entitlements;
        ledger.serializeEntitlements ( *account, entitlements );
        jsonOut.set ( "entitlements", entitlements );
        
        jsonOut.set ( "feeSchedule", ToJSONSerializer::toJSON ( ledger.getTransactionFeeSchedule ()));
        
        shared_ptr < const MinerInfo > minerInfo = accountODBM.mMinerInfo.get ();
        if ( minerInfo ) {
            jsonOut.set ( "miner", ToJSONSerializer::toJSON ( *minerInfo ));
        }
    }

    //----------------------------------------------------------------//
    HTTPStatus AbstractMinerAPIRequestHandler_handleRequest ( HTTP::Method method, shared_ptr < Miner > miner, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {
        UNUSED ( method );
        UNUSED ( jsonIn );
        
        ScopedSharedMinerLedgerLock ledger ( miner );
        ledger.seek ( this->optQuery ( "at", ledger.countBlocks ()));
        
        string accountName = this->getMatchString ( "accountName" );
        
        AccountODBM accountODBM ( ledger, accountName );
        if ( !accountODBM ) return Poco::Net::HTTPResponse::HTTP_NOT_FOUND;
        
        AccountDetailsHandler::formatJSON ( ledger, accountODBM, jsonOut );
        
        ScopedSharedMinerStatusLock minerStatus ( miner );
        jsonOut.set ( "minGratuity", minerStatus.mMinimumGratuity );
        
        return Poco::Net::HTTPResponse::HTTP_OK;
    }
};

} // namespace TheWebMinerAPI
} // namespace Volition
#endif
