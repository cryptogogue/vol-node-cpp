// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTIONS_GENESIS_BLOCK_H
#define VOLITION_TRANSACTIONS_GENESIS_BLOCK_H

#include <volition/common.h>
#include <volition/AbstractTransactionBody.h>
#include <volition/Policy.h>

namespace Volition {
namespace Transactions {

//================================================================//
// GenesisBlock
//================================================================//
class GenesisBlock :
    public AbstractTransactionBody {
public:

    TRANSACTION_TYPE ( "GENESIS_BLOCK" )
    TRANSACTION_WEIGHT ( 0 )
    TRANSACTION_MATURITY ( 0 )

    string      mIdentity;

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractTransactionBody::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "identity",      this->mIdentity );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractTransactionBody::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "identity",      this->mIdentity );
    }

    //----------------------------------------------------------------//
    bool AbstractTransactionBody_apply ( Ledger& ledger ) const override {
    
        if ( ledger.isGenesis ()) {
            ledger.setIdentity ( this->mIdentity );
        }
        return false;
    }
};

} // namespace Transactions
} // namespace Volition
#endif
