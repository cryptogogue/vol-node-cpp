// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTIONS_SET_MINIMUM_GRATUITY_H
#define VOLITION_TRANSACTIONS_SET_MINIMUM_GRATUITY_H

#include <volition/common.h>
#include <volition/AbstractTransactionBody.h>
#include <volition/Digest.h>
#include <volition/Miner.h>
#include <volition/Schema.h>

namespace Volition {
namespace Transactions {

//================================================================//
// SetMinimumGratuity
//================================================================//
class SetMinimumGratuity :
    public AbstractTransactionBody {
public:

    TRANSACTION_TYPE ( "SET_MINIMUM_GRATUITY" )
    TRANSACTION_WEIGHT ( 0 )
    TRANSACTION_MATURITY ( 0 )
    TRANSACTION_CONTROL ( Miner::CONTROL_CONFIG )

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractTransactionBody::AbstractSerializable_serializeFrom ( serializer );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractTransactionBody::AbstractSerializable_serializeTo ( serializer );
    }

    //----------------------------------------------------------------//
    TransactionResult AbstractTransactionBody_apply ( TransactionContext& context ) const override {
        UNUSED ( context );
        
        return true;
    }
    
    //----------------------------------------------------------------//
    TransactionResult AbstractTransactionBody_control ( Miner& miner  ) const override {
        UNUSED ( miner );
        
        return true;
    }
    
    //----------------------------------------------------------------//
    u64 AbstractTransactionBody_nonce () const override {
    
        return 0;
    }
};

} // namespace Transactions
} // namespace Volition
#endif
