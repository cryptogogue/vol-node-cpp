// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_STATE_H
#define VOLITION_STATE_H

#include <volition/common.h>
#include <volition/MinerInfo.h>

namespace Volition {

class Policy;
class TransactionMakerSignature;

//================================================================//
// KeyAndPolicy
//================================================================//
class KeyAndPolicy {
private:

    friend class State;
    friend class Account;
    
    Poco::Crypto::ECKey mKey;
    
    //----------------------------------------------------------------//
    KeyAndPolicy ( const Poco::Crypto::ECKey& key ) :
        mKey ( key ) {
    }
};

//================================================================//
// Account
//================================================================//
class Account {
private:

    friend class State;
    
    u64         mBalance;
    u64         mNonce;

    map < string, KeyAndPolicy >  mKeys;

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
    void getKeys ( map < string, Poco::Crypto::ECKey >& keys ) const {
    
        map < string, KeyAndPolicy >::const_iterator keyIt = this->mKeys.cbegin ();
        for ( ; keyIt != this->mKeys.end (); ++keyIt ) {
            keys.insert ( pair < string, Poco::Crypto::ECKey >( keyIt->first, keyIt->second.mKey )); // because Poco::Crypto::ECKey has no empty constructor
        }
    }
};

//================================================================//
// AccountKey
//================================================================//
class AccountKey {
private:

    friend class State;
    
    Account*            mAccount;
    KeyAndPolicy*       mKeyAndPolicy;
    
    //----------------------------------------------------------------//
    operator bool () const {
        return ( this->mAccount && this->mKeyAndPolicy );
    }
};

//================================================================//
// State
//================================================================//
class State {
private:

    u64                         mHeight;

    map < string, MinerInfo >   mMinerInfo;
    map < string, string >      mMinerURLs;
    map < string, Account >     mAccounts;

    //----------------------------------------------------------------//
    Account*                            getAccount              ( string accountName );
    AccountKey                          getAccountKey           ( string accountName, string keyName );

public:

    //----------------------------------------------------------------//
    bool                                accountPolicy           ( string accountName, const Policy* policy );
    bool                                affirmKey               ( string accountName, string keyName, const Poco::Crypto::ECKey* key, string policyName );
    bool                                checkMakerSignature     ( const TransactionMakerSignature* makerSignature ) const;
    void                                consumeMakerSignature   ( const TransactionMakerSignature* makerSignature );
    bool                                deleteKey               ( string accountName, string keyName );
    bool                                genesisMiner            ( string accountName, u64 amount, string keyName, const Poco::Crypto::ECKey& key, string url );
    const Account*                      getAccount              ( string accountName ) const;
    u64                                 getHeight               () const;
    const map < string, MinerInfo >&    getMinerInfo            () const;
    const MinerInfo*                    getMinerInfo            ( string minerID ) const;
    const map < string, string >&       getMinerURLs            () const;
    bool                                keyPolicy               ( string accountName, string policyName, const Policy* policy );
    bool                                openAccount             ( string accountName, string recipientName, u64 amount, string keyName, const Poco::Crypto::ECKey& key );
    bool                                registerMiner           ( string accountName, string keyName, string url );
    bool                                sendVOL                 ( string accountName, string recipientName, u64 amount );
    void                                setHeight               ( u64 height );
                                        State                   ();
                                        ~State                  ();
};

} // namespace Volition
#endif
