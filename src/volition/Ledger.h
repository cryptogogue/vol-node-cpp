// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_LEDGER_H
#define VOLITION_LEDGER_H

#include <volition/common.h>
#include <volition/MinerInfo.h>
#include <volition/serialization/Serialization.h>

namespace Volition {

class Runtime;
class Policy;
class TransactionMakerSignature;

//================================================================//
// AssetIdentifier
//================================================================//
class AssetIdentifier :
     public AbstractSerializable {
private:

    string      mClassName;             // class this asset is based on.
    string      mSpecialization;        // identifier of the asset specialization (if any).
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
    
        serializer.serialize ( "className",         this->mClassName );
        serializer.serialize ( "specialization",    this->mSpecialization );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
    
        serializer.serialize ( "className",         this->mClassName );
        serializer.serialize ( "specialization",    this->mSpecialization );
    }
};

//================================================================//
// BulkAssetIdentifier
//================================================================//
class BulkAssetIdentifier :
    public AbstractSerializable {
private:

    string      mClassName;             // class this asset is based on.
    u64         mQuantity;              // quantity.
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
    
        serializer.serialize ( "className",         this->mClassName );
        serializer.serialize ( "quantity",          this->mQuantity );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
    
        serializer.serialize ( "className",         this->mClassName );
        serializer.serialize ( "quantity",          this->mQuantity );
    }
};


//================================================================//
// AssetSpecialization
//================================================================//
class AssetSpecialization {
private:

    string      mClass;                 // class this asset is based on.
    string      mOwnerAccountName;      // account name of current owner.
    string      mBody;
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
// Schema
//================================================================//
class Schema {
public:

    string      mJSON;
    string      mLua;
};

//================================================================//
// Ledger
//================================================================//
class Ledger :
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
    bool                            awardAsset              ( string accountName, string schemaName, string assetName, int quantity );
    bool                            affirmKey               ( string accountName, string keyName, const CryptoKey& key, string policyName );
    bool                            checkMakerSignature     ( const TransactionMakerSignature* makerSignature ) const;
    bool                            deleteKey               ( string accountName, string keyName );
    bool                            genesisMiner            ( string accountName, u64 amount, string keyName, const CryptoKey& key, string url );
    AccountKey                      getAccountKey           ( string accountName, string keyName ) const;
    VersionedValue < Account >      getAccount              ( string accountName ) const;
    VersionedValue < MinerInfo >    getMinerInfo            ( string accountName ) const;
    map < string, MinerInfo >       getMiners               () const;
    MinerURLMap                     getMinerURLs            () const;
    list < Schema >                 getSchemas              () const;
    void                            incrementNonce          ( const TransactionMakerSignature* makerSignature );
    bool                            keyPolicy               ( string accountName, string policyName, const Policy* policy );
    bool                            openAccount             ( string accountName, string recipientName, u64 amount, string keyName, const CryptoKey& key );
    bool                            publishSchema           ( string schemaName, string json, string lua );
    bool                            registerMiner           ( string accountName, string keyName, string url );
    void                            reset                   ();
    bool                            sendVOL                 ( string accountName, string recipientName, u64 amount );
                                    Ledger                  ();
                                    Ledger                  ( Ledger& other );
                                    ~Ledger                 ();
};

} // namespace Volition
#endif
