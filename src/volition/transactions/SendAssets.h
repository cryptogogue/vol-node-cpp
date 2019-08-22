// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTIONS_SEND_ASSETS_H
#define VOLITION_TRANSACTIONS_SEND_ASSETS_H

#include <volition/common.h>
#include <volition/AbstractTransactionBody.h>
#include <volition/Policy.h>

namespace Volition {
namespace Transactions {

//================================================================//
// SendAssets
//================================================================//
class SendAssets :
    public AbstractTransactionBody {
public:

    TRANSACTION_TYPE ( "SEND_ASSETS" )
    TRANSACTION_WEIGHT ( 1 )
    TRANSACTION_MATURITY ( 0 )

    SerializableVector < string >   mAssetIdentifiers;

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractTransactionBody::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "assetIdentifiers",          this->mAssetIdentifiers );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractTransactionBody::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "assetIdentifiers",          this->mAssetIdentifiers );
    }

    //----------------------------------------------------------------//
    bool AbstractTransactionBody_apply ( Ledger& ledger ) const override {
        UNUSED ( ledger );
        
        //return ledger.sendVOL ( this->mMaker->getAccountName (), this->mAccountName, this->mAmount );
        return false;
    }
};

} // namespace Transactions
} // namespace Volition
#endif
