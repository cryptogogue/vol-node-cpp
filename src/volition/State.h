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
    
    //----------------------------------------------------------------//
    u64 getNonce () const {
        return this->mNonce;
    }
};

//================================================================//
// AccountKey
//================================================================//
class AccountKey {
private:

    friend class State;
    
    VersionedValue < Account >  mAccount;
    const KeyAndPolicy*         mKeyAndPolicy;
    
    //----------------------------------------------------------------//
    operator bool () const {
        return ( this->mAccount && this->mKeyAndPolicy );
    }
};

//================================================================//
// Schema
//================================================================//
class Schema {
public:

    string      mJSON;
    string      mLua;
};

//================================================================//
// State
//================================================================//
class State :
    public VersionedStore {
private:

    static constexpr const char* ACCOUNT            = "account";
    static constexpr const char* MINERS             = "miners";
    static constexpr const char* MINER_INFO         = "minerInfo";
    static constexpr const char* MINER_URLS         = "minerUrls";
    static constexpr const char* SCHEMA_COUNT       = "schemaCount";
    static constexpr const char* SCHEMA_PREFIX      = "schema.";

    //----------------------------------------------------------------//
    static string                   getSchemaKey            ( int schemaCount );
    static string                   prefixKey               ( string prefix, string key );
    void                            setAccount              ( string accountName, const Account& account );
    void                            setMinerInfo            ( string accountName, const MinerInfo& minerInfo );

public:

    typedef VersionedValue < map < string, string >> MinerURLMap;

    //----------------------------------------------------------------//
    bool                            accountPolicy           ( string accountName, const Policy* policy );
    bool                            affirmKey               ( string accountName, string keyName, const CryptoKey& key, string policyName );
    bool                            checkMakerSignature     ( const TransactionMakerSignature* makerSignature ) const;
    void                            consumeMakerSignature   ( const TransactionMakerSignature* makerSignature );
    bool                            deleteKey               ( string accountName, string keyName );
    bool                            genesisMiner            ( string accountName, u64 amount, string keyName, const CryptoKey& key, string url );
    AccountKey                      getAccountKey           ( string accountName, string keyName ) const;
    VersionedValue < Account >      getAccount              ( string accountName ) const;
    VersionedValue < MinerInfo >    getMinerInfo            ( string accountName ) const;
    map < string, MinerInfo >       getMiners               () const;
    MinerURLMap                     getMinerURLs            () const;
    list < Schema >                 getSchemas              () const;
    bool                            keyPolicy               ( string accountName, string policyName, const Policy* policy );
    bool                            openAccount             ( string accountName, string recipientName, u64 amount, string keyName, const CryptoKey& key );
    bool                            publishSchema           ( string json, string lua );
    bool                            registerMiner           ( string accountName, string keyName, string url );
    void                            reset                   ();
    bool                            sendVOL                 ( string accountName, string recipientName, u64 amount );
                                    State                   ();
                                    State                   ( State& other );
                                    ~State                  ();
};

} // namespace Volition
#endif
