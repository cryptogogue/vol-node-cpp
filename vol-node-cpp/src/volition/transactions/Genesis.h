// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTIONS_GENESIS_H
#define VOLITION_TRANSACTIONS_GENESIS_H

#include <volition/common.h>
#include <volition/AbstractTransactionBody.h>
#include <volition/Policy.h>

namespace Volition {
namespace Transactions {

//================================================================//
// GenesisAccount
//================================================================//
class GenesisAccount :
    public AbstractSerializable {
public:

    string      mName;
    CryptoKey   mKey;
    u64         mGrant;
    string      mURL;

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        
        serializer.serialize ( "name",      this->mName );
        serializer.serialize ( "grant",     this->mGrant  );
        serializer.serialize ( "key",       this->mKey );
        serializer.serialize ( "url",       this->mURL );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        
        serializer.serialize ( "name",      this->mName );
        serializer.serialize ( "grant",     this->mGrant  );
        serializer.serialize ( "key",       this->mKey );
        serializer.serialize ( "url",       this->mURL );
    }
};

//================================================================//
// Genesis
//================================================================//
class Genesis :
    public AbstractTransactionBody {
public:

    TRANSACTION_TYPE ( "GENESIS" )
    TRANSACTION_WEIGHT ( 0 )
    TRANSACTION_MATURITY ( 0 )

    string                                  mIdentity;
    SerializableVector < GenesisAccount >   mAccounts;

    //----------------------------------------------------------------//
    TransactionResult genesis ( Ledger& ledger ) const {
    
        ledger.setIdentity ( this->mIdentity );
        
        for ( size_t i = 0; i < this->mAccounts.size (); ++i ) {
            const GenesisAccount& account = this->mAccounts [ i ];
            
            if ( !ledger.newAccount ( account.mName, account.mGrant, Ledger::MASTER_KEY_NAME, account.mKey, Policy (), Policy ())) return false;
            if ( account.mURL.size () > 0 ) {
                if ( !ledger.registerMiner ( account.mName, Ledger::MASTER_KEY_NAME, account.mURL )) return false;
            }
        }
        return true;
    }

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractTransactionBody::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "identity",      this->mIdentity );
        serializer.serialize ( "accounts",      this->mAccounts );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractTransactionBody::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "identity",      this->mIdentity );
        serializer.serialize ( "accounts",      this->mAccounts );
    }

    //----------------------------------------------------------------//
    TransactionResult AbstractTransactionBody_apply ( TransactionContext& context ) const override {
        UNUSED ( context );
        return false;
    }
};

} // namespace Transactions
} // namespace Volition
#endif