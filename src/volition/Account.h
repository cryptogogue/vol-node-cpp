// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_ACCOUNT_H
#define VOLITION_ACCOUNT_H

#include <volition/common.h>
#include <volition/CryptoKey.h>
#include <volition/serialization/Serialization.h>

namespace Volition {

//================================================================//
// KeyInfo
//================================================================//
class KeyInfo :
    public AbstractSerializable {
public:
    
    string mAccountName;
    string mKeyName;

    //----------------------------------------------------------------//
    KeyInfo () {
    }
    
    //----------------------------------------------------------------//
    KeyInfo ( string accountName, string keyName ) :
        mAccountName ( accountName ),
        mKeyName ( keyName ) {
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
    
        serializer.serialize ( "accountName",       this->mAccountName );
        serializer.serialize ( "keyName",           this->mKeyName );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
    
        serializer.serialize ( "accountName",       this->mAccountName );
        serializer.serialize ( "keyName",           this->mKeyName );
    }
};

//================================================================//
// KeyAndPolicy
//================================================================//
class KeyAndPolicy :
    public AbstractSerializable {
public:

    friend class Ledger;
    friend class Account;
    
    CryptoKey mKey;

    //----------------------------------------------------------------//
    KeyAndPolicy () {
    }
    
    //----------------------------------------------------------------//
    KeyAndPolicy ( const CryptoKey& key ) :
        mKey ( key ) {
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
    
        serializer.serialize ( "key",               this->mKey );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
    
        serializer.serialize ( "key",               this->mKey );
    }
};

//================================================================//
// Account
//================================================================//
class Account :
    public AbstractSerializable {
private:

    friend class Ledger;
    
    u64         mBalance;
    u64         mNonce;

    SerializableMap < string, KeyAndPolicy >  mKeys;

public:

    //----------------------------------------------------------------//
    Account () :
        mBalance ( 0 ),
        mNonce ( 0 ) {
    }

    //----------------------------------------------------------------//
    u64 getBalance () const {
        return this->mBalance;
    }

    //----------------------------------------------------------------//
    void getKeys ( map < string, CryptoKey >& keys ) const {
    
        map < string, KeyAndPolicy >::const_iterator keyIt = this->mKeys.cbegin ();
        for ( ; keyIt != this->mKeys.end (); ++keyIt ) {
            keys [ keyIt->first ] = keyIt->second.mKey;
        }
    }
    
    //----------------------------------------------------------------//
    u64 getNonce () const {
        return this->mNonce;
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
    
        serializer.serialize ( "balance",           this->mBalance );
        serializer.serialize ( "nonce",             this->mNonce );
        serializer.serialize ( "keys",              this->mKeys );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
    
        serializer.serialize ( "balance",           this->mBalance );
        serializer.serialize ( "nonce",             this->mNonce );
        serializer.serialize ( "keys",              this->mKeys );
    }
};

//================================================================//
// AccountKey
//================================================================//
class AccountKey {
public:
    
    shared_ptr < Account >  mAccount;
    const KeyAndPolicy*     mKeyAndPolicy;
    
    //----------------------------------------------------------------//
    operator bool () const {
        return ( this->mAccount && this->mKeyAndPolicy );
    }
};

} // namespace Volition
#endif
