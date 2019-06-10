// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_LEDGER_H
#define VOLITION_LEDGER_H

#include <volition/common.h>
#include <volition/Entropy.h>
#include <volition/MinerInfo.h>
#include <volition/serialization/Serialization.h>

namespace Volition {

class Asset;
class AssetFieldValue;
class AssetIdentifier;
class Block;
class Policy;
class Schema;
class TransactionMakerSignature;

//================================================================//
// UnfinishedBlock
//================================================================//
class UnfinishedBlock :
    public AbstractSerializable {
public:

    u64         mBlockID;       // ID of the block containing unfinished transactions
    u64         mMaturity;      // Height of the chain when transactions should be applied
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
    
        serializer.serialize ( "blockID",       this->mBlockID );
        serializer.serialize ( "maturity",      this->mMaturity );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
    
        serializer.serialize ( "blockID",       this->mBlockID );
        serializer.serialize ( "maturity",      this->mMaturity );
    }
};

//================================================================//
// UnfinishedBlockList
//================================================================//
class UnfinishedBlockList :
    public AbstractSerializable {
public:

    typedef SerializableList < UnfinishedBlock >::const_iterator    Iterator;

    SerializableList < UnfinishedBlock >    mBlocks;
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
    
        serializer.serialize ( "blocks",      this->mBlocks );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
    
        serializer.serialize ( "blocks",      this->mBlocks );
    }
};

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
private:

    friend class Ledger;
    
    shared_ptr < Account >  mAccount;
    const KeyAndPolicy*     mKeyAndPolicy;
    
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
    static constexpr const char* UNFINISHED         = "unfinished";

    //static constexpr const char* ASSET_INSTANCE_KEY_FMT_SSD     = "%s.inventory.%s.%d";

protected:

    //----------------------------------------------------------------//
    static string                   formatKeyForAccountInventory    ( string accountName );
    static string                   formatKeyForAsset               ( const AssetIdentifier& identifier );
    static string                   formatKeyForAssetCounter        ( string assetType );
    static string                   formatKeyForAssetDefinition     ( string assetType );
    static string                   formatKeyForAssetField          ( const AssetIdentifier& identifier, string fieldName );
    static string                   formatKeyForSchemaCount         ();
    static string                   formatSchemaKey                 ( int schemaCount );
    static string                   formatSchemaKey                 ( string schemaName );
    static string                   prefixKey                       ( string prefix, string key );
    void                            setAccount                      ( string accountName, const Account& account );
    void                            setMinerInfo                    ( string accountName, const MinerInfo& minerInfo );

public:

    typedef SerializableMap < string, string > MinerURLMap;

    //----------------------------------------------------------------//
    bool                            accountPolicy           ( string accountName, const Policy* policy );
    bool                            awardAsset              ( Schema& schema, string accountName, string assetName, int quantity );
    bool                            affirmKey               ( string accountName, string keyName, const CryptoKey& key, string policyName );
    bool                            checkMakerSignature     ( const TransactionMakerSignature* makerSignature ) const;
    bool                            deleteKey               ( string accountName, string keyName );
    bool                            genesisMiner            ( string accountName, u64 amount, string keyName, const CryptoKey& key, string url );
    shared_ptr < Account >          getAccount              ( string accountName ) const;
    AccountKey                      getAccountKey           ( string accountName, string keyName ) const;
    shared_ptr < Asset >            getAsset                ( const AssetIdentifier& identifier ) const;
    shared_ptr < Block >            getBlock                ( size_t height ) const;
    shared_ptr < Block >            getTopBlock             () const;
    Entropy                         getEntropy              () const;
//    Inventory                       getInventory            ( string accountName ) const;
    shared_ptr < KeyInfo >          getKeyInfo              ( string keyID ) const;
    shared_ptr < MinerInfo >        getMinerInfo            ( string accountName ) const;
    map < string, MinerInfo >       getMiners               () const;
    shared_ptr < MinerURLMap >      getMinerURLs            () const;
    shared_ptr < Schema >           getSchema               ( string schemaName ) const;
    list < Schema >                 getSchemas              () const;
    UnfinishedBlockList             getUnfinished           ();
    void                            incrementNonce          ( const TransactionMakerSignature* makerSignature );
    bool                            keyPolicy               ( string accountName, string policyName, const Policy* policy );
    bool                            openAccount             ( string accountName, string recipientName, u64 amount, string keyName, const CryptoKey& key );
    bool                            publishSchema           ( string schemaName, const Schema& schema );
    bool                            registerMiner           ( string accountName, string keyName, string url );
    void                            reset                   ();
    bool                            sendVOL                 ( string accountName, string recipientName, u64 amount );
    bool                            setAssetFieldValue      ( const AssetIdentifier& identifier, string fieldName, const AssetFieldValue& field );
    void                            setBlock                ( const Block& block );
    void                            setEntropyString        ( string entropy );
    void                            setUnfinished           ( const UnfinishedBlockList& unfinished );
                                    Ledger                  ();
                                    Ledger                  ( Ledger& other );
                                    ~Ledger                 ();

    //----------------------------------------------------------------//
    template < typename TYPE >
    shared_ptr < TYPE > getJSONSerializableObject ( string key ) const {

        return Ledger::getJSONSerializableObject < TYPE >( *this, key );
    }
    
    //----------------------------------------------------------------//
    template < typename TYPE >
    static shared_ptr < TYPE > getJSONSerializableObject ( const VersionedStoreSnapshot& snapshot, string key ) {
    
        string json = snapshot.getValueOrFallback < string >( key, "" );
        if ( json.size () > 0 ) {
            shared_ptr < TYPE > object = make_shared < TYPE >();
            FromJSONSerializer::fromJSONString ( *object, json );
            return object;
        }
        return NULL;
    }
    
    //----------------------------------------------------------------//
    template < typename TYPE >
    void setJSONSerializableObject ( string key, const TYPE& object ) {
    
        string json = ToJSONSerializer::toJSONString ( object );
        this->setValue < string >( key, json );
    }
};

} // namespace Volition
#endif
