// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_LEDGER_H
#define VOLITION_LEDGER_H

#include <volition/common.h>
#include <volition/Account.h>
#include <volition/Asset.h>
#include <volition/Entropy.h>
#include <volition/FormatLedgerKey.h>
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

    static constexpr const char* MASTER_KEY_NAME    = "master";

protected:

    //----------------------------------------------------------------//
    void                            setAccount              ( string accountName, const Account& account );
    void                            setMinerInfo            ( string accountName, const MinerInfo& minerInfo );

public:

    typedef SerializableMap < string, string > MinerURLMap;

    //----------------------------------------------------------------//
    bool                            accountPolicy           ( string accountName, const Policy* policy );
    bool                            awardAsset              ( string accountName, string assetType, int quantity );
    bool                            affirmKey               ( string accountName, string keyName, const CryptoKey& key, string policyName );
    bool                            deleteKey               ( string accountName, string keyName );
    bool                            genesisMiner            ( string accountName, u64 balance, const CryptoKey& key, string url );
    shared_ptr < Account >          getAccount              ( string accountName ) const;
    Account::Index                  getAccountIndex         ( string accountName ) const;
    AccountKey                      getAccountKey           ( string accountName, string keyName ) const;
    shared_ptr < AccountKeyLookup > getAccountKeyLookup     ( string keyID ) const;
    string                          getAccountName          ( Account::Index accountIndex ) const;
    shared_ptr < Asset >            getAsset                ( Asset::Index index ) const;
    shared_ptr < Block >            getBlock                () const;
    shared_ptr < Block >            getBlock                ( size_t height ) const;
    Entropy                         getEntropy              () const;
    string                          getIdentity             () const;
    SerializableList < Asset >      getInventory            ( string accountName ) const;
    shared_ptr < AssetMethod >      getMethod               ( string methodName ) const;
    shared_ptr < MinerInfo >        getMinerInfo            ( string accountName ) const;
    map < string, MinerInfo >       getMiners               () const;
    shared_ptr < MinerURLMap >      getMinerURLs            () const;
    shared_ptr < Schema >           getSchema               ( Schema::Index schemaIndex ) const;
    Schema::Index                   getSchemaCount          () const;
    Schema::Index                   getSchemaIndex          ( string schemaName ) const;
    string                          getSchemaString         ( Schema::Index schemaIndex ) const;
    UnfinishedBlockList             getUnfinished           ();
    void                            incrementNonce          ( const TransactionMaker& makerSignature );
    bool                            invoke                  ( string accountName, const AssetMethodInvocation& invocation );
    static bool                     isAccountName           ( string accountName );
    static bool                     isChildName             ( string accountName );
    static bool                     isSuffix                ( string suffix );
    bool                            isGenesis               () const;
    bool                            keyPolicy               ( string accountName, string policyName, const Policy* policy );
                                    Ledger                  ();
                                    Ledger                  ( Ledger& other );
                                    ~Ledger                 ();
    bool                            newAccount              ( string accountName, u64 balance, const CryptoKey& key );
    bool                            publishSchema           ( string accountName, const Schema& schema, string schemaString );
    bool                            registerMiner           ( string accountName, string keyName, string url );
    bool                            renameAccount           ( string accountName, string revealedName, Digest nameHash, Digest nameSecret );
    void                            reset                   ();
    bool                            sendVOL                 ( string accountName, string recipientName, u64 amount );
    bool                            setAssetFieldValue      ( Asset::Index index, string fieldName, const AssetFieldValue& field );
    void                            setBlock                ( const Block& block );
    void                            setEntropyString        ( string entropy );
    bool                            setIdentity             ( string identity );
    void                            setUnfinished           ( const UnfinishedBlockList& unfinished );
    bool                            sponsorAccount          ( string sponsorName, string suffix, u64 grant, const CryptoKey& key );
    bool                            verify                  ( const AssetMethodInvocation& invocation ) const;

    //----------------------------------------------------------------//
    template < typename TYPE >
    shared_ptr < TYPE > getObjectOrNull ( LedgerKey key ) const {

        return Ledger::getObjectOrNull < TYPE >( *this, key );
    }
    
    //----------------------------------------------------------------//
    template < typename TYPE >
    static shared_ptr < TYPE > getObjectOrNull ( const VersionedStoreSnapshot& snapshot, LedgerKey key ) {
    
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
    void setObject ( LedgerKey key, const TYPE& object ) {
    
        string json = ToJSONSerializer::toJSONString ( object );
        this->setValue < string >( key, json );
    }
};

} // namespace Volition
#endif
