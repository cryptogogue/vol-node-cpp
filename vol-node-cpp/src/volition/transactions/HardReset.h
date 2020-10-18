// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTIONS_PUBLISH_HARD_RESET_H
#define VOLITION_TRANSACTIONS_PUBLISH_HARD_RESET_H

#include <volition/common.h>
#include <volition/AbstractTransactionBody.h>
#include <volition/Digest.h>
#include <volition/Miner.h>
#include <volition/Schema.h>

namespace Volition {
namespace Transactions {

//================================================================//
// HardReset
//================================================================//
class HardReset :
    public AbstractTransactionBody {
public:

    TRANSACTION_TYPE ( "HARD_RESET" )
    TRANSACTION_WEIGHT ( 0 )
    TRANSACTION_MATURITY ( 0 )
    TRANSACTION_CONTROL ( Miner::CONTROL_ADMIN )

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
        
        printf ( "CONTROL: doing hard reset.\n" );
        
        miner.reset ();
        miner.shutdown ( true );
        
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
