// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINERAPI_KEYACCOUNTDETAILSHANDLER_H
#define VOLITION_WEBMINERAPI_KEYACCOUNTDETAILSHANDLER_H

#include <volition/AccountODBM.h>
#include <volition/AssetODBM.h>
#include <volition/Block.h>
#include <volition/SemiBlockingMinerAPIRequestHandler.h>
#include <volition/TheTransactionBodyFactory.h>

namespace Volition {
namespace WebMinerAPI {

//================================================================//
// AccountDetailsHandler
//================================================================//
class AccountDetailsHandler :
    public SemiBlockingMinerAPIRequestHandler {
public:

    SUPPORTED_HTTP_METHODS ( HTTP::GET )

    //----------------------------------------------------------------//
    static void formatJSON ( const AbstractLedger& ledger, AccountODBM& accountODBM, Poco::JSON::Object& jsonOut ) {
        
        LGN_LOG_SCOPE ( VOL_FILTER_HTTP, INFO, __PRETTY_FUNCTION__ );
        
        // account's "primary" name
        LGN_LOG ( VOL_FILTER_HTTP, INFO, "account name" );
        string accountName = accountODBM.mName.get ();
        
        LGN_LOG ( VOL_FILTER_HTTP, INFO, "account body" );
        shared_ptr < const Account > account = accountODBM.mBody.get ();
        
        // get the account JSON
        LGN_LOG ( VOL_FILTER_HTTP, INFO, "account json" );
        Poco::JSON::Object::Ptr accountJSON = ToJSONSerializer::toJSON ( *account ).extract < Poco::JSON::Object::Ptr >();
        
        // decorate with virtual fields
        LGN_LOG ( VOL_FILTER_HTTP, INFO, "adding fields" );
        accountJSON->set ( "name",              accountName );
        accountJSON->set ( "index",             ( u64 )accountODBM.mAccountID );
        accountJSON->set ( "assetCount",        accountODBM.mAssetCount.get ( 0 ));
        accountJSON->set ( "inventoryNonce",    accountODBM.mInventoryNonce.get ( 0 ));
        accountJSON->set ( "nonce",             accountODBM.mTransactionNonce.get ( 0 ));
        accountJSON->set ( "height",            ledger.getHeight ());
        
        jsonOut.set ( "account", accountJSON );
        
        LGN_LOG ( VOL_FILTER_HTTP, INFO, "getting entitlements" );
        ToJSONSerializer entitlements;
        ledger.serializeEntitlements ( *account, entitlements );
        jsonOut.set ( "entitlements", entitlements );
        
        LGN_LOG ( VOL_FILTER_HTTP, INFO, "getting fee schedule" );
        TransactionFeeSchedule feeSchedule = ledger.getTransactionFeeSchedule ();
        jsonOut.set ( "feeSchedule", ToJSONSerializer::toJSON ( feeSchedule ));
        
        LGN_LOG ( VOL_FILTER_HTTP, INFO, "getting miner info" );
        shared_ptr < const MinerInfo > minerInfo = accountODBM.mMinerInfo.get ();
        if ( minerInfo ) {
            jsonOut.set ( "miner", ToJSONSerializer::toJSON ( *minerInfo ));
        }
    }

    //----------------------------------------------------------------//
    HTTPStatus SemiBlockingMinerAPIRequestHandler_handleRequest ( HTTP::Method method, AbstractLedger& ledger, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {
        UNUSED ( method );
        UNUSED ( jsonIn );
    
        LGN_LOG_SCOPE ( VOL_FILTER_HTTP, INFO, __PRETTY_FUNCTION__ );
    
        LGN_LOG ( VOL_FILTER_HTTP, INFO, "getting account name" );
        string accountName = this->getMatchString ( "accountName" );
        
        AccountODBM accountODBM ( ledger, accountName );
        if ( !accountODBM ) return Poco::Net::HTTPResponse::HTTP_NOT_FOUND;
        
        AccountDetailsHandler::formatJSON ( ledger, accountODBM, jsonOut );
        jsonOut.set ( "minGratuity",    this->mStatus.mMinimumGratuity );
        return Poco::Net::HTTPResponse::HTTP_OK;
    }
};

} // namespace TheWebMinerAPI
} // namespace Volition
#endif
