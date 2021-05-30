// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTIONS_BUYASSETS_H
#define VOLITION_TRANSACTIONS_BUYASSETS_H

#include <volition/common.h>
#include <volition/AbstractTransactionBody.h>
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

    string              mIdentifier;
    u64                 mPrice;

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractTransactionBody::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "identifier",        this->mIdentifier );
        serializer.serialize ( "price",             this->mPrice );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractTransactionBody::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "identifier",        this->mIdentifier );
        serializer.serialize ( "price",             this->mPrice );
    }

    //----------------------------------------------------------------//
    TransactionResult AbstractTransactionBody_apply ( TransactionContext& context ) const override {
                
        if ( !context.mKeyEntitlements.check ( KeyEntitlements::BUY_ASSETS )) return "Permission denied.";
        
        AbstractLedger& ledger = context.mLedger;
        
        AssetID assetID ( this->mIdentifier );
        if ( assetID.mIndex == AssetID::NULL_INDEX ) return "Invalid asset identifier.";
        
        AssetODBM assetODBM ( ledger, assetID );
        if ( !assetODBM ) return "Asset not found.";
        
        OfferID offerID = assetODBM.mOffer.get ();
        if ( offerID.mIndex == AssetID::NULL_INDEX ) return "Asset not offered for sale.";

        OfferODBM offerODBM ( ledger, assetODBM.mOffer.get ());
        if ( !offerODBM ) return "Asset marked for sale, but no offer found.";
        
        AccountODBM sellerODBM ( ledger, offerODBM.mSeller.get ());
        if ( !sellerODBM ) return "Seller not found.";

        AccountODBM buyerODBM ( ledger, context.mAccountID );
        if ( !buyerODBM ) return "Buyer not found.";

        if ( context.mAccountID == sellerODBM.mAccountID ) return "Cannot buy assets from self; cancel sale instead.";

        SerializableVector < AssetID::Index > assetIDs;
        offerODBM.mAssetIdentifiers.get ( assetIDs );

        ledger.transferAssets ( sellerODBM, buyerODBM, AssetListAdapter ( assetIDs.data (), assetIDs.size ()), context.mTime );

        // TODO: change status instead
        offerODBM.mSeller.set ( OfferID::NULL_INDEX );

        buyerODBM.subFunds (  this->mPrice );
        sellerODBM.addFunds (  this->mPrice );
        
        context.pushTransactionLogEntry ( sellerODBM.mAccountID );
        
        return true;
    }
    
    //----------------------------------------------------------------//
    u64 AbstractTransactionBody_sendVOL () const override {
    
        return this->mPrice;
    }
};

} // namespace Transactions
} // namespace Volition
#endif
