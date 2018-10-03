// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTION_TRANSACTIONS_H
#define VOLITION_TRANSACTION_TRANSACTIONS_H

#include <volition/common.h>
#include <volition/AbstractTransaction.h>
#include <volition/Policy.h>

// ACCOUNT_POLICY (account, policy)
// AFFIRM_KEY (account name, key name, policy name, (opt)key)
// DELETE_KEY (account name, key name)
// GENESIS_MINER ( account name, key name, url, amount)
// KEY_POLICY (account, policy, policy name)
// OPEN_ACCOUNT (account name, master key, key name)
// REGISTER_MINER (account name, url)
// SEND_VOL (from, to)

namespace Volition {
namespace Transaction {

//================================================================//
// AccountPolicy
//================================================================//
class AccountPolicy :
    public AbstractTransaction {
public:

    TRANSACTION_TYPE ( "ACCOUNT_POLICY" )
    TRANSACTION_WEIGHT ( 1 )

    string                                  mPolicyName;
    SerializableUniquePtr < Policy >        mPolicy;

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractTransaction::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "policy",        this->mPolicy );
        serializer.serialize ( "policyName",    this->mPolicyName );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractTransaction::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "policy",        this->mPolicy );
        serializer.serialize ( "policyName",    this->mPolicyName );
    }

    //----------------------------------------------------------------//
    bool AbstractTransaction_apply ( State& state ) const override {
        return true;
    }
};

//================================================================//
// AffirmKey
//================================================================//
class AffirmKey :
    public AbstractTransaction {
public:

    TRANSACTION_TYPE ( "AFFIRM_KEY" )
    TRANSACTION_WEIGHT ( 1 )

    string      mKeyName;
    CryptoKey   mKey;
    string      mPolicyName;

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractTransaction::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "key",           this->mKey );
        serializer.serialize ( "keyName",       this->mKeyName );
        serializer.serialize ( "policyName",    this->mPolicyName );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractTransaction::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "key",           this->mKey );
        serializer.serialize ( "keyName",       this->mKeyName );
        serializer.serialize ( "policyName",    this->mPolicyName );
    }

    //----------------------------------------------------------------//
    bool AbstractTransaction_apply ( State& state ) const override {
    
        return state.affirmKey ( this->mMakerSignature->getAccountName (), this->mKeyName, this->mKey, this->mPolicyName );
    }
};

//================================================================//
// GenesisMiner
//================================================================//
class GenesisMiner :
    public AbstractTransaction {
public:

    TRANSACTION_TYPE ( "GENESIS_MINER" )
    TRANSACTION_WEIGHT ( 1 )

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
    bool AbstractTransaction_apply ( State& state ) const override {
    
        assert ( this->mKey );
        return state.genesisMiner ( this->mAccountName, this->mAmount, this->mKeyName, this->mKey, this->mURL );
    }
};

//================================================================//
// KeyPolicy
//================================================================//
class KeyPolicy :
    public AbstractTransaction {
public:

    TRANSACTION_TYPE ( "KEY_POLICY" )
    TRANSACTION_WEIGHT ( 1 )

    string                                  mPolicyName;
    SerializableUniquePtr < Policy >        mPolicy;

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractTransaction::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "policy",        this->mPolicy );
        serializer.serialize ( "policyName",    this->mPolicyName );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractTransaction::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "policy",        this->mPolicy );
        serializer.serialize ( "policyName",    this->mPolicyName );
    }

    //----------------------------------------------------------------//
    bool AbstractTransaction_apply ( State& state ) const override {
        return true;
    }
};

//================================================================//
// OpenAccount
//================================================================//
class OpenAccount :
    public AbstractTransaction {
public:

    TRANSACTION_TYPE ( "OPEN_ACCOUNT" )
    TRANSACTION_WEIGHT ( 1 )

    string      mAccountName;   // user provided name of account
    CryptoKey   mKey;           // master key
    string      mKeyName;       // name of master key
    u64         mAmount;        // amount to fund

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractTransaction::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "accountName",   this->mAccountName );
        serializer.serialize ( "amount",        this->mAmount  );
        serializer.serialize ( "key",           this->mKey );
        serializer.serialize ( "keyName",       this->mKeyName );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractTransaction::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "accountName",   this->mAccountName );
        serializer.serialize ( "amount",        this->mAmount  );
        serializer.serialize ( "key",           this->mKey );
        serializer.serialize ( "keyName",       this->mKeyName );
    }

    //----------------------------------------------------------------//
    bool AbstractTransaction_apply ( State& state ) const override {
        
        assert ( this->mKey );
        return state.openAccount ( this->mMakerSignature->getAccountName (), this->mAccountName, this->mAmount, this->mKeyName, this->mKey );
    }
};

//================================================================//
// RegisterMiner
//================================================================//
class RegisterMiner :
    public AbstractTransaction {
public:

    TRANSACTION_TYPE ( "REGISTER_MINER" )
    TRANSACTION_WEIGHT ( 1 )

    string                                  mURL;

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractTransaction::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "url",           this->mURL );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractTransaction::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "url",           this->mURL );
    }

    //----------------------------------------------------------------//
    bool AbstractTransaction_apply ( State& state ) const override {
    
        return state.registerMiner ( this->mMakerSignature->getAccountName (), this->mMakerSignature->getKeyName (), this->mURL );
    }
};

//================================================================//
// SendVOL
//================================================================//
class SendVOL :
    public AbstractTransaction {
public:

    TRANSACTION_TYPE ( "SEND_VOL" )
    TRANSACTION_WEIGHT ( 1 )

    string                                  mAccountName; // receiving account
    u64                                     mAmount;

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractTransaction::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "accountName",   this->mAccountName );
        serializer.serialize ( "amount",        this->mAmount  );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractTransaction::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "accountName",   this->mAccountName );
        serializer.serialize ( "amount",        this->mAmount  );
    }

    //----------------------------------------------------------------//
    bool AbstractTransaction_apply ( State& state ) const override {
        
        return state.sendVOL ( this->mMakerSignature->getAccountName (), this->mAccountName, this->mAmount );
    }
};

} // namespace Transaction
} // namespace Volition
#endif
