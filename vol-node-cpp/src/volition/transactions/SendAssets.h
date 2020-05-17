// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTIONS_SEND_ASSETS_H
#define VOLITION_TRANSACTIONS_SEND_ASSETS_H

#include <volition/common.h>
#include <volition/AbstractTransactionBody.h>
#include <volition/AssetODBM.h>
#include <volition/Format.h>
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

    string                          mAccountName;
    SerializableVector < string >   mAssetIdentifiers;

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractTransactionBody::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "accountName",               this->mAccountName );
        serializer.serialize ( "assetIdentifiers",          this->mAssetIdentifiers );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractTransactionBody::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "accountName",               this->mAccountName );
        serializer.serialize ( "assetIdentifiers",          this->mAssetIdentifiers );
    }

    //----------------------------------------------------------------//
    TransactionResult AbstractTransactionBody_apply ( TransactionContext& context ) const override {
                
        if ( !context.mKeyEntitlements.check ( KeyEntitlements::SEND_ASSETS )) return "Permission denied.";
        
        return context.mLedger.transferAssets (
            context.mAccount.mIndex,
            context.mLedger.getAccountIndex ( this->mAccountName ),
            this->mAssetIdentifiers.data (),
            this->mAssetIdentifiers.size ()
        );
    }
};

} // namespace Transactions
} // namespace Volition
#endif
