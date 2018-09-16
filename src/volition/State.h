// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_STATE_H
#define VOLITION_STATE_H

#include <volition/common.h>
#include <volition/MinerInfo.h>
#include <volition/VersionedStore.h>

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
    
    CryptoKey mKey;

public:

    //----------------------------------------------------------------//
    KeyAndPolicy () {
    }
    
    //----------------------------------------------------------------//
    KeyAndPolicy ( const CryptoKey& key ) :
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
    void getKeys ( map < string, CryptoKey >& keys ) const {
    
        map < string, KeyAndPolicy >::const_iterator keyIt = this->mKeys.cbegin ();
        for ( ; keyIt != this->mKeys.end (); ++keyIt ) {
            keys [ keyIt->first ] = keyIt->second.mKey;
        }
    }
};

//================================================================//
// AccountKey
//================================================================//
class AccountKey {
private:

    friend class State;
    
    const Account*          mAccount;
    const KeyAndPolicy*     mKeyAndPolicy;
    
    //----------------------------------------------------------------//
    operator bool () const {
        return ( this->mAccount && this->mKeyAndPolicy );
    }
};

//================================================================//
// State
//================================================================//
class State :
    public VersionedStore {
private:

    static constexpr const char* ACCOUNT        = "account";
    static constexpr const char* MINERS         = "miners";
    static constexpr const char* MINER_INFO     = "minerInfo";
    static constexpr const char* MINER_URLS     = "minerUrls";

    //----------------------------------------------------------------//
    static string                       prefixKey               ( string prefix, string key );
    void                                setAccount              ( string accountName, const Account& account );
    void                                setMinerInfo            ( string accountName, const MinerInfo& minerInfo );

public:

    //----------------------------------------------------------------//
    bool                                accountPolicy           ( string accountName, const Policy* policy );
    bool                                affirmKey               ( string accountName, string keyName, const CryptoKey& key, string policyName );
    bool                                checkMakerSignature     ( const TransactionMakerSignature* makerSignature ) const;
    void                                consumeMakerSignature   ( const TransactionMakerSignature* makerSignature );
    bool                                deleteKey               ( string accountName, string keyName );
    bool                                genesisMiner            ( string accountName, u64 amount, string keyName, const CryptoKey& key, string url );
    AccountKey                          getAccountKey           ( string accountName, string keyName ) const;
    const Account*                      getAccountOrNil         ( string accountName ) const;
    const MinerInfo*                    getMinerInfoOrNil       ( string accountName ) const;
    map < string, MinerInfo >           getMiners               () const;
    const map < string, string >&       getMinerURLs            () const;
    bool                                keyPolicy               ( string accountName, string policyName, const Policy* policy );
    bool                                openAccount             ( string accountName, string recipientName, u64 amount, string keyName, const CryptoKey& key );
    bool                                registerMiner           ( string accountName, string keyName, string url );
//    void                                reset                   ();
    bool                                sendVOL                 ( string accountName, string recipientName, u64 amount );
                                        State                   ();
                                        ~State                  ();
};

} // namespace Volition
#endif
