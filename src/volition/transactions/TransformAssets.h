// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTIONS_TRANSFORM_ASSETS_H
#define VOLITION_TRANSACTIONS_TRANSFORM_ASSETS_H

#include <volition/common.h>
#include <volition/AbstractTransaction.h>
#include <volition/AbstractSingleSignerTransaction.h>
#include <volition/Policy.h>

namespace Volition {
namespace Transactions {

//================================================================//
// TransformAssets
//================================================================//
class TransformAssets :
    public AbstractSingleSignerTransaction {
public:

    TRANSACTION_TYPE ( "TRANSFORM_ASSETS" )
    TRANSACTION_WEIGHT ( 1 )

    string                                  mSchemaName;
    string                                  mRuleName;
    SerializableVector < AssetIdentifier >  mAssetIdentifiers;

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractSingleSignerTransaction::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "schemaName",            this->mSchemaName );
        serializer.serialize ( "ruleName",              this->mRuleName );
        serializer.serialize ( "assetIdentifiers",      this->mAssetIdentifiers  );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractSingleSignerTransaction::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "schemaName",            this->mSchemaName );
        serializer.serialize ( "ruleName",              this->mRuleName );
        serializer.serialize ( "assetIdentifiers",      this->mAssetIdentifiers  );
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
