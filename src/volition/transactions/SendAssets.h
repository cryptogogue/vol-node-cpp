// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTIONS_SEND_ASSETS_H
#define VOLITION_TRANSACTIONS_SEND_ASSETS_H

#include <volition/common.h>
#include <volition/AbstractSingleSignerTransaction.h>
#include <volition/Policy.h>

namespace Volition {
namespace Transactions {

//================================================================//
// SendAssets
//================================================================//
class SendAssets :
    public AbstractSingleSignerTransaction {
public:

    TRANSACTION_TYPE ( "SEND_ASSETS" )
    TRANSACTION_WEIGHT ( 1 )
    TRANSACTION_MATURITY ( 0 )

    SerializableVector < AssetIdentifier >          mAssetIdentifiers;
    SerializableVector < BulkAssetIdentifier >      mBulkAssetItdentifiers;

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractSingleSignerTransaction::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "assetIdentifiers",          this->mAssetIdentifiers );
        serializer.serialize ( "bulkAssetIdentifiers",      this->mBulkAssetItdentifiers  );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractSingleSignerTransaction::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "assetIdentifiers",          this->mAssetIdentifiers );
        serializer.serialize ( "bulkAssetIdentifiers",      this->mBulkAssetItdentifiers  );
    }

    //----------------------------------------------------------------//
    bool AbstractTransaction_apply ( Ledger& ledger ) const override {
        
        //return ledger.sendVOL ( this->mMakerSignature->getAccountName (), this->mAccountName, this->mAmount );
        return false;
    }
};

} // namespace Transactions
} // namespace Volition
#endif
