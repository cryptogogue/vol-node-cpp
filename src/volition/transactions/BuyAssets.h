// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTIONS_BUYASSETS_H
#define VOLITION_TRANSACTIONS_BUYASSETS_H

#include <volition/common.h>
#include <volition/AbstractTransactionBody.h>
#include <volition/AssetODBM.h>
#include <volition/Format.h>
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
        
        return context.mLedger.buyAssets (
            context.mAccountID,
            this->mIdentifier,
            this->mPrice,
            context.mTime
        );
    }
    
    //----------------------------------------------------------------//
    u64 AbstractTransactionBody_sendVOL () const override {
    
        return this->mPrice;
    }
};

} // namespace Transactions
} // namespace Volition
#endif
