// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTIONS_SELL_ASSETS_H
#define VOLITION_TRANSACTIONS_SELL_ASSETS_H

#include <volition/common.h>
#include <volition/AbstractTransaction.h>
#include <volition/Policy.h>

namespace Volition {
namespace Transactions {

//================================================================//
// SellAssets
//================================================================//
class SellAssets :
    public AbstractTransaction {
public:

    TRANSACTION_TYPE ( "SELL_ASSETS" )
    TRANSACTION_WEIGHT ( 1 )
    TRANSACTION_MATURITY ( 0 )

    SerializableUniquePtr < TransactionMakerSignature >     mBuyerSignature;
    SerializableUniquePtr < TransactionMakerSignature >     mSellerSignature;

    SerializableVector < string >                           mAssetIdentifiers;
    u64                                                     mPrice;

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractTransaction::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "buyerSignature",            this->mBuyerSignature );
        serializer.serialize ( "sellerSignature",           this->mSellerSignature );
        serializer.serialize ( "assetIdentifiers",          this->mAssetIdentifiers );
        serializer.serialize ( "price",                     this->mPrice  );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractTransaction::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "buyerSignature",            this->mBuyerSignature );
        serializer.serialize ( "sellerSignature",           this->mSellerSignature );
        serializer.serialize ( "assetIdentifiers",          this->mAssetIdentifiers );
        serializer.serialize ( "price",                     this->mPrice  );
    }

    //----------------------------------------------------------------//
    bool AbstractTransaction_apply ( Ledger& ledger ) const override {
        
        //return ledger.sendVOL ( this->mMakerSignature->getAccountName (), this->mAccountName, this->mAmount );
        return false;
    }
    
    //----------------------------------------------------------------//
    void AbstractTransaction_incrementNonce ( Ledger& ledger ) const override {

        ledger.incrementNonce ( this->mBuyerSignature.get ());
        ledger.incrementNonce ( this->mSellerSignature.get ());
    }
    
    //----------------------------------------------------------------//
    bool AbstractTransaction_verify  ( const Ledger& ledger ) const override {

        const TransactionMakerSignature* buyerSignature = this->mBuyerSignature.get ();
        const TransactionMakerSignature* sellerSignature = this->mSellerSignature.get ();
        
        return ( buyerSignature && sellerSignature && ledger.checkMakerSignature ( buyerSignature ) && ledger.checkMakerSignature ( sellerSignature ));
    }
};

} // namespace Transactions
} // namespace Volition
#endif
