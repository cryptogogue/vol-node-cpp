// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTIONS_OFFERASSETS_H
#define VOLITION_TRANSACTIONS_OFFERASSETS_H

#include <volition/common.h>
#include <volition/AbstractTransactionBody.h>
#include <volition/AssetODBM.h>
#include <volition/Format.h>
#include <volition/Policy.h>

namespace Volition {
namespace Transactions {

//================================================================//
// OfferAssets
//================================================================//
class OfferAssets :
    public AbstractTransactionBody {
public:

    TRANSACTION_TYPE ( "OFFER_ASSETS" )
    TRANSACTION_WEIGHT ( 1 )
    TRANSACTION_MATURITY ( 0 )

    // TODO: this is a 1.0 of the auction transaction. will evenutally need to include a delegated auctioneer and a profit share.

    u64                             mMinimumPrice;
    SerializableTime                mExpiration;
    SerializableVector < string >   mAssetIdentifiers;

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractTransactionBody::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "minimumPrice",              this->mMinimumPrice );
        serializer.serialize ( "expiration",                this->mExpiration );
        serializer.serialize ( "assetIdentifiers",          this->mAssetIdentifiers );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractTransactionBody::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "minimumPrice",              this->mMinimumPrice );
        serializer.serialize ( "expiration",                this->mExpiration );
        serializer.serialize ( "assetIdentifiers",          this->mAssetIdentifiers );
    }

    //----------------------------------------------------------------//
    TransactionResult AbstractTransactionBody_apply ( TransactionContext& context ) const override {
                
        if ( !context.mKeyEntitlements.check ( KeyEntitlements::OFFER_ASSETS )) return "Permission denied.";
        
        return context.mLedger.offerAssets (
            context.mAccountID,
            this->mMinimumPrice,
            this->mExpiration,
            AssetListAdapter (
                this->mAssetIdentifiers.data (),
                this->mAssetIdentifiers.size ()
            ),
            context.mTime
        );
    }
};

} // namespace Transactions
} // namespace Volition
#endif
