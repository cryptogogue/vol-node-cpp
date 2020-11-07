// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTIONS_SETFEESCHEDULE_H
#define VOLITION_TRANSACTIONS_SETFEESCHEDULE_H

#include <volition/common.h>
#include <volition/AbstractTransactionBody.h>
#include <volition/FeeSchedule.h>

namespace Volition {
namespace Transactions {

//================================================================//
// SetFeeSchedule
//================================================================//
class SetFeeSchedule :
    public AbstractTransactionBody {
public:

    TRANSACTION_TYPE ( "SET_FEE_SCHEDULE" )
    TRANSACTION_WEIGHT ( 1 )
    TRANSACTION_MATURITY ( 0 )

    FeeSchedule     mFeeSchedule;

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractTransactionBody::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "feeSchedule",       this->mFeeSchedule  );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractTransactionBody::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "feeSchedule",       this->mFeeSchedule  );
    }

    //----------------------------------------------------------------//
    TransactionResult AbstractTransactionBody_apply ( TransactionContext& context ) const override {
                
        if ( !context.mKeyEntitlements.check ( KeyEntitlements::SET_FEE_SCHEDULE )) return "Permission denied.";
        context.mLedger.setFeeSchedule ( this->mFeeSchedule );
        return true;
    }
};

} // namespace Transactions
} // namespace Volition
#endif
