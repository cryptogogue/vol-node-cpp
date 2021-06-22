// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTIONS_STAMP_ASSETS_H
#define VOLITION_TRANSACTIONS_STAMP_ASSETS_H

#include <volition/common.h>
#include <volition/AbstractTransactionBody.h>
#include <volition/AssetODBM.h>
#include <volition/Format.h>
#include <volition/Policy.h>

namespace Volition {
namespace Transactions {

//================================================================//
// StampAssets
//================================================================//
class StampAssets :
    public AbstractTransactionBody {
public:
    
    TRANSACTION_TYPE ( "STAMP_ASSETS" )
    TRANSACTION_WEIGHT ( 1 )
    TRANSACTION_MATURITY ( 0 )
    
    string                          mStamp;
    u64                             mPrice;
    u64                             mVersion;
    SerializableVector < string >   mAssetIdentifiers;
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractTransactionBody::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "stamp",                     this->mStamp );
        serializer.serialize ( "price",                     this->mPrice );
        serializer.serialize ( "version",                   this->mVersion );
        serializer.serialize ( "assetIdentifiers",          this->mAssetIdentifiers );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractTransactionBody::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "stamp",                     this->mStamp );
        serializer.serialize ( "price",                     this->mPrice );
        serializer.serialize ( "version",                   this->mVersion );
        serializer.serialize ( "assetIdentifiers",          this->mAssetIdentifiers );
    }
    
    //----------------------------------------------------------------//
    TransactionResult AbstractTransactionBody_apply ( TransactionContext& context ) const override {
    
        // TODO: check requested version and price in case stamp changed!
        
        return context.mLedger.stampAssets (
            context.mAccountID,
            AssetID::decode ( this->mStamp ),
            this->mPrice,
            this->mVersion,
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
