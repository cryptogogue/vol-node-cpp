// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_ACCOUNTLOGENTRY_H
#define VOLITION_ACCOUNTLOGENTRY_H

#include <volition/common.h>
#include <volition/Accessors.h>
#include <volition/serialization/Serialization.h>

namespace Volition {

//================================================================//
// AccountLogEntry
//================================================================//
class AccountLogEntry :
    public AbstractSerializable {
private:

    friend class Ledger_Account;

    u64     mBlockHeight;
    u64     mTransactionIndex;

public:

    GET ( u64,          BlockHeight,                mBlockHeight )
    GET ( u64,          TransactionIndex,           mTransactionIndex )
    
    //----------------------------------------------------------------//
    AccountLogEntry () :
        mBlockHeight ( 0 ),
        mTransactionIndex ( 0 ) {
    }
    
    //----------------------------------------------------------------//
    AccountLogEntry ( u64 blockHeight, u64 transactionIndex ) :
        mBlockHeight ( blockHeight ),
        mTransactionIndex ( transactionIndex ) {
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
    
        serializer.serialize ( "blockHeight",           this->mBlockHeight );
        serializer.serialize ( "transactionIndex",      this->mTransactionIndex );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
    
        serializer.serialize ( "blockHeight",           this->mBlockHeight );
        serializer.serialize ( "transactionIndex",      this->mTransactionIndex );
    }
};

} // namespace Volition
#endif
