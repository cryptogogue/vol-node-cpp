// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTIONS_CANCELOFFER_H
#define VOLITION_TRANSACTIONS_CANCELOFFER_H

#include <volition/common.h>
#include <volition/AbstractTransaction.h>
#include <volition/AssetODBM.h>
#include <volition/Format.h>
#include <volition/Policy.h>

namespace Volition {
namespace Transactions {

//================================================================//
// CancelOffer
//================================================================//
class CancelOffer :
    public AbstractTransaction {
public:

    TRANSACTION_TYPE ( "CANCEL_OFFER" )
    TRANSACTION_WEIGHT ( 1 )
    TRANSACTION_MATURITY ( 0 )

    string              mIdentifier;

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractTransaction::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "identifier",        this->mIdentifier );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractTransaction::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "identifier",        this->mIdentifier );
    }

    //----------------------------------------------------------------//
    TransactionResult AbstractTransactionBody_apply ( TransactionContext& context ) const override {
        
        return context.mLedger.cancelOffer (
            context.mAccountID,
            this->mIdentifier,
            context.mTime
        );
    }
};

} // namespace Transactions
} // namespace Volition
#endif
