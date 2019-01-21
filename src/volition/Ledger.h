// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_LEDGER_H
#define VOLITION_LEDGER_H

#include <volition/common.h>
#include <volition/Entropy.h>
#include <volition/Inventory.h>
#include <volition/MinerInfo.h>
#include <volition/serialization/Serialization.h>

namespace Volition {

class Block;
class Policy;
class Schema;
class TransactionMakerSignature;

//================================================================//
// UnfinishedBlock
//================================================================//
class UnfinishedBlock {
public:

    size_t      mBlockID;       // ID of the block containing unfinished transactions
    size_t      mMaturity;      // Height of the chain when transactions should be applied
};

//================================================================//
// UnfinishedBlockList
//================================================================//
class UnfinishedBlockList {
public:

    typedef list < UnfinishedBlock >::const_iterator    Iterator;

    list < UnfinishedBlock >    mBlocks;
};

//================================================================//
// KeyInfo
//================================================================//
class KeyInfo {
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
};

//================================================================//
// KeyAndPolicy
//================================================================//
class KeyAndPolicy {
private:

    friend class Ledger;
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

    friend class Ledger;
    
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

    friend class Ledger;
    
    VersionedValue < Account >  mAccount;
    const KeyAndPolicy*         mKeyAndPolicy;
    
    //----------------------------------------------------------------//
    operator bool () const {
        return ( this->mAccount && this->mKeyAndPolicy );
    }
};

//================================================================//
// Ledger
//================================================================//
class Ledger :
    public VersionedStore {
public:

    static constexpr const char* ACCOUNT            = "account";
    static constexpr const char* BLOCK_KEY          = "block";
    static constexpr const char* KEY_ID             = "keyID.";
    static constexpr const char* ENTROPY            = "entropy";
    static constexpr const char* MINERS             = "miners";
    static constexpr const char* MINER_INFO         = "minerInfo";
    static constexpr const char* MINER_URLS         = "minerUrls";
    static constexpr const char* SCHEMA_COUNT       = "schemaCount";
    static constexpr const char* SCHEMA_PREFIX      = "schema.";
    static constexpr const char* UNFINISHED         = "unfinished";

    static constexpr const char* INVENTORY_KEY_FMT_S        = "%s.inventory";
    //static constexpr const char* ASSET_INSTANCE_KEY_FMT_SSD     = "%s.inventory.%s.%d";

protected:

    //----------------------------------------------------------------//
    static string                   getInventoryKey         ( string accountName );
    static string                   getSchemaKey            ( int schemaCount );
    static string                   getSchemaNameKey        ( string schemaName );
    static string                   prefixKey               ( string prefix, string key );
    void                            setAccount              ( string accountName, const Account& account );
    void                            setMinerInfo            ( string accountName, const MinerInfo& minerInfo );

public:

    typedef VersionedValue < map < string, string >> MinerURLMap;

    //----------------------------------------------------------------//
    bool                            accountPolicy           ( string accountName, const Policy* policy );
    bool                            awardAsset              ( Schema& schema, string accountName, string assetName, int quantity );
    bool                            affirmKey               ( string accountName, string keyName, const CryptoKey& key, string policyName );
    bool                            checkMakerSignature     ( const TransactionMakerSignature* makerSignature ) const;
    bool                            deleteKey               ( string accountName, string keyName );
    bool                            genesisMiner            ( string accountName, u64 amount, string keyName, const CryptoKey& key, string url );
    VersionedValue < Account >      getAccount              ( string accountName ) const;
    AccountKey                      getAccountKey           ( string accountName, string keyName ) const;
    VersionedValue < Block >        getBlock                ( size_t height ) const;
    VersionedValue < Block >        getTopBlock             () const;
    Entropy                         getEntropy              ();
    Inventory                       getInventory            ( string accountName ) const;
    VersionedValue < KeyInfo >      getKeyInfo              ( string keyID ) const;
    VersionedValue < MinerInfo >    getMinerInfo            ( string accountName ) const;
    map < string, MinerInfo >       getMiners               () const;
    MinerURLMap                     getMinerURLs            () const;
    VersionedValue < Schema >       getSchema               ( string schemaName ) const;
    //list < Schema >                 getSchemas              () const;
    UnfinishedBlockList             getUnfinished           ();
    void                            incrementNonce          ( const TransactionMakerSignature* makerSignature );
    bool                            keyPolicy               ( string accountName, string policyName, const Policy* policy );
    bool                            openAccount             ( string accountName, string recipientName, u64 amount, string keyName, const CryptoKey& key );
    bool                            publishSchema           ( string schemaName, const Schema& schema );
    bool                            registerMiner           ( string accountName, string keyName, string url );
    void                            reset                   ();
    bool                            sendVOL                 ( string accountName, string recipientName, u64 amount );
    void                            setBlock                ( const Block& block );
    void                            setEntropy              ( const Entropy& entropy );
    void                            setUnfinished           ( const UnfinishedBlockList& unfinished );
                                    Ledger                  ();
                                    Ledger                  ( Ledger& other );
                                    ~Ledger                 ();
};

} // namespace Volition
#endif
