// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTIONS_GENESIS_MINER_H
#define VOLITION_TRANSACTIONS_GENESIS_MINER_H

#include <volition/common.h>
#include <volition/AbstractTransaction.h>
#include <volition/Policy.h>

namespace Volition {
namespace Transactions {

//================================================================//
// GenesisMiner
//================================================================//
class GenesisMiner :
    public AbstractTransaction {
public:

    TRANSACTION_TYPE ( "GENESIS_MINER" )
    TRANSACTION_WEIGHT ( 1 )
    TRANSACTION_MATURITY ( 0 )

    string      mAccountName;
    CryptoKey   mKey;
    string      mKeyName;
    u64         mAmount;
    string      mURL;

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractTransaction::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "accountName",   this->mAccountName );
        serializer.serialize ( "amount",        this->mAmount  );
        serializer.serialize ( "key",           this->mKey );
        serializer.serialize ( "keyName",       this->mKeyName );
        serializer.serialize ( "url",           this->mURL );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractTransaction::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "accountName",   this->mAccountName );
        serializer.serialize ( "amount",        this->mAmount  );
        serializer.serialize ( "key",           this->mKey );
        serializer.serialize ( "keyName",       this->mKeyName );
        serializer.serialize ( "url",           this->mURL );
    }

    //----------------------------------------------------------------//
    bool AbstractTransaction_apply ( Ledger& ledger ) const override {
    
        assert ( this->mKey );
        return ledger.genesisMiner ( this->mAccountName, this->mAmount, this->mKeyName, this->mKey, this->mURL );
    }
    
    //----------------------------------------------------------------//
    void AbstractTransaction_incrementNonce ( Ledger& ledger ) const override {
    }
    
    //----------------------------------------------------------------//
    bool AbstractTransaction_verify  ( const Ledger& ledger ) const override {

        return ( ledger.getVersion () == 0 );
    }
};

} // namespace Transactions
} // namespace Volition
#endif
