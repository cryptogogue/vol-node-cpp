// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTIONS_SETPAYOUTPOLICY_H
#define VOLITION_TRANSACTIONS_SETPAYOUTPOLICY_H

#include <volition/common.h>
#include <volition/AbstractTransaction.h>
#include <volition/PayoutPolicy.h>

namespace Volition {
namespace Transactions {

//================================================================//
// SetPayoutPolicy
//================================================================//
class SetPayoutPolicy :
    public AbstractTransaction {
public:

    TRANSACTION_TYPE ( "SET_PAYOUT_POLICY" )
    TRANSACTION_WEIGHT ( 1 )
    TRANSACTION_MATURITY ( 0 )

    PayoutPolicy    mPayoutPolicy;

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractTransaction::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "payoutPolicy",      this->mPayoutPolicy );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractTransaction::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "payoutPolicy",      this->mPayoutPolicy );
    }

    //----------------------------------------------------------------//
    TransactionResult AbstractTransactionBody_apply ( TransactionContext& context ) const override {
                
        if ( !context.mKeyEntitlements.check ( KeyEntitlements::SET_PAYOUT_POLICY )) return "Permission denied.";
        return context.mLedger.setPayoutPolicy ( this->mPayoutPolicy );
    }
};

} // namespace Transactions
} // namespace Volition
#endif
