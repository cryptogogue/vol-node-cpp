// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTIONS_BUYASSETS_H
#define VOLITION_TRANSACTIONS_BUYASSETS_H

#include <volition/common.h>
#include <volition/AbstractTransactionBody.h>
#include <volition/AssetTransferDetails.h>
#include <volition/AssetODBM.h>
#include <volition/Format.h>
#include <volition/IndexID.h>
#include <volition/OfferODBM.h>
#include <volition/Policy.h>

namespace Volition {
namespace Transactions {

//================================================================//
// BuyAssets
//================================================================//
class BuyAssets :
    public AbstractTransactionBody {
public:

    TRANSACTION_TYPE ( "BUY_ASSETS" )
    TRANSACTION_WEIGHT ( 1 )
    TRANSACTION_MATURITY ( 0 )

    OfferID             mOfferID;
    u64                 mPrice;

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractTransactionBody::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "offerID",           this->mOfferID );
        serializer.serialize ( "price",             this->mPrice );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractTransactionBody::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "offerID",           this->mOfferID );
        serializer.serialize ( "price",             this->mPrice );
    }

    //----------------------------------------------------------------//
    TransactionResult AbstractTransactionBody_apply ( TransactionContext& context ) const override {
                
        if ( !context.mKeyEntitlements.check ( KeyEntitlements::BUY_ASSETS )) return "Permission denied.";
        
        AbstractLedger& ledger = context.mLedger;

        OfferODBM offerODBM ( ledger, this->mOfferID );
        if ( !offerODBM ) return "No offer found.";
        
        AccountODBM sellerODBM ( ledger, offerODBM.mSeller.get ());
        if ( !sellerODBM ) return "Seller not found.";
        
        AccountODBM buyerODBM ( ledger, context.mAccountID );
        if ( !buyerODBM ) return "Buyer not found.";
        
        AccountID buyerID = offerODBM.mBuyer.get ();
        if ( buyerID == buyerODBM.mAccountID ) return "Buyer already purchased offer.";
        if ( buyerID != AccountID::NULL_INDEX ) return "Offer already has a buyer.";
        
        time_t expiration = Format::fromISO8601 ( offerODBM.mExpiration.get ());
        if ( expiration <= context.mTime ) return "Offer expired.";

        if ( context.mAccountID == sellerODBM.mAccountID ) return "Cannot buy assets from self; cancel sale instead.";

        SerializableVector < AssetID::Index > assetIDs;
        offerODBM.mAssetIdentifiers.get ( assetIDs );

        ledger.transferAssets ( sellerODBM, buyerODBM, AssetListAdapter ( assetIDs.data (), assetIDs.size ()), context.mTime );

        offerODBM.mBuyer.set ( buyerODBM.mAccountID );

        if ( context.mRelease > 0 ) {
            buyerODBM.subFunds ( this->mPrice );
        }
        sellerODBM.addFunds ( this->mPrice );
        
        context.pushAccountLogEntry ( sellerODBM.mAccountID );
        
        return true;
    }
    
    //----------------------------------------------------------------//
    TransactionDetailsPtr AbstractTransactionBody_getDetails ( const AbstractLedger& ledger ) const override {
        
        OfferODBM offerODBM ( ledger, this->mOfferID );
        if ( !offerODBM ) return NULL;
        
        AccountODBM buyerODBM ( ledger, offerODBM.mBuyer.get ());
        if ( !buyerODBM ) return NULL;
        
        AccountODBM sellerODBM ( ledger, offerODBM.mSeller.get ());
        if ( !sellerODBM ) return NULL;
        
        SerializableVector < AssetID::Index > assetIDs;
        offerODBM.mAssetIdentifiers.get ( assetIDs );
        
        shared_ptr < AssetTransferDetails > details = make_shared < AssetTransferDetails >();
        
        SerializableVector < AssetID::Index >::const_iterator assetIDIt = assetIDs.cbegin ();
        for ( ; assetIDIt != assetIDs.cend (); ++assetIDIt ) {
            shared_ptr < const Asset > asset = AssetODBM ( ledger, *assetIDIt ).getAsset ();
            if ( !asset ) return NULL;
            details->mAssets.push_back ( asset );
        }
        
        details->mFrom      = sellerODBM.mName.get ();
        details->mFromID    = sellerODBM.mAccountID;
        
        details->mTo        = buyerODBM.mName.get ();
        details->mToID      = buyerODBM.mAccountID;
        
        return details;
    }
    
    //----------------------------------------------------------------//
    u64 AbstractTransactionBody_getVOL ( const TransactionContext& context ) const override {
        UNUSED ( context );
    
        return this->mPrice;
    }
};

} // namespace Transactions
} // namespace Volition
#endif
