// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTIONS_SETMONETARYPOLICY_H
#define VOLITION_TRANSACTIONS_SETMONETARYPOLICY_H

#include <volition/common.h>
#include <volition/AbstractTransaction.h>
#include <volition/MonetaryPolicy.h>

namespace Volition {
namespace Transactions {

//================================================================//
// SetMonetaryPolicy
//================================================================//
class SetMonetaryPolicy :
    public AbstractTransaction {
public:

    TRANSACTION_TYPE ( "SET_MONETARY_POLICY" )
    TRANSACTION_WEIGHT ( 1 )
    TRANSACTION_MATURITY ( 0 )

    MonetaryPolicy      mMonetaryPolicy;

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractTransaction::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "monetaryPolicy",    this->mMonetaryPolicy  );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractTransaction::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "monetaryPolicy",    this->mMonetaryPolicy  );
    }

    //----------------------------------------------------------------//
    TransactionResult AbstractTransactionBody_apply ( TransactionContext& context ) const override {
                
        if ( !context.mKeyEntitlements.check ( KeyEntitlements::SET_MONETARY_POLICY )) return "Permission denied.";
        context.mLedger.setMonetaryPolicy ( this->mMonetaryPolicy );
        return true;
    }
};

} // namespace Transactions
} // namespace Volition
#endif
