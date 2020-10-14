// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTIONS_GENESIS_H
#define VOLITION_TRANSACTIONS_GENESIS_H

#include <volition/common.h>
#include <volition/AbstractTransactionBody.h>
#include <volition/MinerInfo.h>
#include <volition/Policy.h>

namespace Volition {
namespace Transactions {

//================================================================//
// GenesisAccount
//================================================================//
class GenesisAccount :
    public AbstractSerializable {
public:

    string              mName;
    CryptoPublicKey     mKey;
    u64                 mGrant;
    SerializableSharedConstPtr < MinerInfo > mMinerInfo;

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        
        serializer.serialize ( "name",          this->mName );
        serializer.serialize ( "grant",         this->mGrant  );
        serializer.serialize ( "key",           this->mKey );
        serializer.serialize ( "minerInfo",     this->mMinerInfo );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        
        serializer.serialize ( "name",          this->mName );
        serializer.serialize ( "grant",         this->mGrant  );
        serializer.serialize ( "key",           this->mKey );
        serializer.serialize ( "minerInfo",     this->mMinerInfo );
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
            
            u64 grant = ledger.createVOL ( account.mGrant );
            
            if ( !ledger.newAccount ( account.mName, grant, Ledger::MASTER_KEY_NAME, account.mKey, Policy (), Policy ())) return false;
            if ( account.mMinerInfo ) {
                if ( !ledger.registerMiner (
                    ledger.getAccountID ( account.mName ),
                    *account.mMinerInfo
                )) return false;
            }
        }
        return true;
    }

    //----------------------------------------------------------------//
    void pushAccount ( const GenesisAccount& genesisAccount ) {
    
        this->mAccounts.push_back ( genesisAccount );
    }
    
    //----------------------------------------------------------------//
    void setIdentity ( string identity ) {
    
        this->mIdentity = identity;
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
