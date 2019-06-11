// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTIONS_REGISTER_MINER_H
#define VOLITION_TRANSACTIONS_REGISTER_MINER_H

#include <volition/common.h>
#include <volition/AbstractSingleSignerTransaction.h>
#include <volition/Policy.h>

namespace Volition {
namespace Transactions {

//================================================================//
// RegisterMiner
//================================================================//
class RegisterMiner :
    public AbstractSingleSignerTransaction {
public:

    TRANSACTION_TYPE ( "REGISTER_MINER" )
    TRANSACTION_WEIGHT ( 1 )
    TRANSACTION_MATURITY ( 0 )

    string  mURL;

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractSingleSignerTransaction::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "url",           this->mURL );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractSingleSignerTransaction::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "url",           this->mURL );
    }

    //----------------------------------------------------------------//
    bool AbstractTransaction_apply ( Ledger& ledger ) const override {
    
        return ledger.registerMiner ( this->mMakerSignature->getAccountName (), this->mMakerSignature->getKeyName (), this->mURL );
    }
};

} // namespace Transactions
} // namespace Volition
#endif
