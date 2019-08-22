// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_LEDGER_H
#define VOLITION_LEDGER_H

#include <volition/common.h>
#include <volition/Account.h>
#include <volition/Asset.h>
#include <volition/Entropy.h>
#include <volition/MinerInfo.h>
#include <volition/serialization/Serialization.h>

namespace Volition {

class AssetFieldValue;
class AssetMethod;
class AssetMethodInvocation;
class Block;
class Policy;
class Schema;
class TransactionMaker;
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

    static constexpr const char* ACCOUNT_HEAD       = "head";
    static constexpr const char* ACCOUNT_TAIL       = "tail";

    static constexpr const char* ASSET_NEXT         = "next";
    static constexpr const char* ASSET_OWNER        = "owner";
    static constexpr const char* ASSET_PREV         = "prev";
    static constexpr const char* ASSET_TYPE         = "type";

    //static constexpr const char* ASSET_INSTANCE_KEY_FMT_SSD     = "%s.inventory.%s.%d";

protected:

    //----------------------------------------------------------------//
    static string                   formatKeyForAccount             ( string accountName, string member );
    static string                   formatKeyForAsset               ( Asset::Index index, string member );
    static string                   formatKeyForAssetCounter        ();
    static string                   formatKeyForAssetDefinition     ( string assetType );
    static string                   formatKeyForAssetField          ( Asset::Index index, string fieldName );
    static string                   formatKeyForAssetMethod         ( string methodName );
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
    bool                            awardAsset              ( string accountName, string assetType, int quantity );
    bool                            affirmKey               ( string accountName, string keyName, const CryptoKey& key, string policyName );
    bool                            checkMaker              ( const TransactionMaker& maker, const Signature& signature ) const;
    bool                            deleteKey               ( string accountName, string keyName );
    bool                            genesisMiner            ( string accountName, u64 amount, string keyName, const CryptoKey& key, string url );
    shared_ptr < Account >          getAccount              ( string accountName ) const;
    AccountKey                      getAccountKey           ( string accountName, string keyName ) const;
    shared_ptr < Asset >            getAsset                ( string assetID ) const;
    shared_ptr < Asset >            getAsset                ( Asset::Index index ) const;
    shared_ptr < Block >            getBlock                ( size_t height ) const;
    Entropy                         getEntropy              () const;
    SerializableList < Asset >      getInventory            ( string accountName ) const;
    shared_ptr < KeyInfo >          getKeyInfo              ( string keyID ) const;
    shared_ptr < AssetMethod >      getMethod               ( string methodName ) const;
    shared_ptr < MinerInfo >        getMinerInfo            ( string accountName ) const;
    map < string, MinerInfo >       getMiners               () const;
    shared_ptr < MinerURLMap >      getMinerURLs            () const;
    shared_ptr < Schema >           getSchema               ( string schemaName ) const;
    list < Schema >                 getSchemas              () const;
    shared_ptr < Block >            getTopBlock             () const;
    UnfinishedBlockList             getUnfinished           ();
    void                            incrementNonce          ( const TransactionMaker& makerSignature );
    bool                            invoke                  ( string accountName, const AssetMethodInvocation& invocation );
    bool                            isGenesis               () const;
    bool                            keyPolicy               ( string accountName, string policyName, const Policy* policy );
                                    Ledger                  ();
                                    Ledger                  ( Ledger& other );
                                    ~Ledger                 ();
    bool                            openAccount             ( string accountName, string recipientName, u64 amount, string keyName, const CryptoKey& key );
    bool                            publishSchema           ( string accountName, string schemaName, const Schema& schema );
    bool                            registerMiner           ( string accountName, string keyName, string url );
    void                            reset                   ();
    bool                            sendVOL                 ( string accountName, string recipientName, u64 amount );
    bool                            setAssetFieldValue      ( Asset::Index index, string fieldName, const AssetFieldValue& field );
    void                            setBlock                ( const Block& block );
    void                            setEntropyString        ( string entropy );
    void                            setUnfinished           ( const UnfinishedBlockList& unfinished );
    bool                            verify                  ( const AssetMethodInvocation& invocation ) const;

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
