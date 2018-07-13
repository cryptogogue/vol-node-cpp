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
    void AbstractSerializable_serialize ( AbstractSerializer& serializer ) override {
        AbstractTransaction::AbstractSerializable_serialize ( serializer );
        
        serializer.serialize ( "policyName",    this->mPolicyName );
        serializer.serialize ( "policy",        this->mPolicy );
    }

    //----------------------------------------------------------------//
    void AbstractTransaction_apply ( State& state ) const override {
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

    string                                  mKeyName;
    unique_ptr < Poco::Crypto::ECKey >      mKey;
    string                                  mPolicyName;

    //----------------------------------------------------------------//
    void AbstractSerializable_serialize ( AbstractSerializer& serializer ) override {
        AbstractTransaction::AbstractSerializable_serialize ( serializer );
        
        serializer.serialize ( "keyName",       this->mKeyName );
        serializer.serialize ( "policyName",    this->mPolicyName );
        serializer.serialize ( "key",           this->mKey );
    }

    //----------------------------------------------------------------//
    void AbstractTransaction_apply ( State& state ) const override {
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

    string                                  mAccountName;
    unique_ptr < Poco::Crypto::ECKey >      mKey;
    string                                  mKeyName;
    u64                                     mAmount;
    string                                  mURL;

    //----------------------------------------------------------------//
    void AbstractSerializable_serialize ( AbstractSerializer& serializer ) override {
        AbstractTransaction::AbstractSerializable_serialize ( serializer );
        
        serializer.serialize ( "accountName",   this->mAccountName );
        serializer.serialize ( "keyName",       this->mKeyName );
        serializer.serialize ( "amount",        this->mAmount  );
        serializer.serialize ( "key",           this->mKey );
        serializer.serialize ( "url",           this->mURL );
    }

    //----------------------------------------------------------------//
    void AbstractTransaction_apply ( State& state ) const override {
    
        state.registerMiner ( MinerInfo ( this->mAccountName, this->mURL, *this->mKey ));
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
    void AbstractSerializable_serialize ( AbstractSerializer& serializer ) override {
        AbstractTransaction::AbstractSerializable_serialize ( serializer );
        
        serializer.serialize ( "policyName",    this->mPolicyName );
        serializer.serialize ( "policy",        this->mPolicy );
    }

    //----------------------------------------------------------------//
    void AbstractTransaction_apply ( State& state ) const override {
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

    string                                  mAccountName;   // user provided name of account
    unique_ptr < Poco::Crypto::ECKey >      mKey;           // master key
    string                                  mKeyName;       // name of master key
    u64                                     mAmount;        // amount to fund

    //----------------------------------------------------------------//
    void AbstractSerializable_serialize ( AbstractSerializer& serializer ) override {
        AbstractTransaction::AbstractSerializable_serialize ( serializer );
        
        serializer.serialize ( "accountName",   this->mAccountName );
        serializer.serialize ( "keyName",       this->mKeyName );
        serializer.serialize ( "amount",        this->mAmount  );
        serializer.serialize ( "key",           this->mKey );
    }

    //----------------------------------------------------------------//
    void AbstractTransaction_apply ( State& state ) const override {
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
    void AbstractSerializable_serialize ( AbstractSerializer& serializer ) override {
        AbstractTransaction::AbstractSerializable_serialize ( serializer );
        
        serializer.serialize ( "url",           this->mURL );
    }

    //----------------------------------------------------------------//
    void AbstractTransaction_apply ( State& state ) const override {
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
    void AbstractSerializable_serialize ( AbstractSerializer& serializer ) override {
        AbstractTransaction::AbstractSerializable_serialize ( serializer );
        
        serializer.serialize ( "accountName",   this->mAccountName );
        serializer.serialize ( "amount",        this->mAmount  );
    }

    //----------------------------------------------------------------//
    void AbstractTransaction_apply ( State& state ) const override {
    }
};

} // namespace Transaction
} // namespace Volition
#endif
