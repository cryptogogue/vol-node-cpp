// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_LEDGER_H
#define VOLITION_LEDGER_H

#include <volition/common.h>
#include <volition/Account.h>
#include <volition/AccountEntitlements.h>
#include <volition/AccountKeyLookup.h>
#include <volition/Asset.h>
#include <volition/Entropy.h>
#include <volition/FormatLedgerKey.h>
#include <volition/KeyEntitlements.h>
#include <volition/MinerInfo.h>
#include <volition/serialization/Serialization.h>

namespace Volition {

class AccountEntitlements;
class AssetFieldValue;
class AssetMethod;
class AssetMethodInvocation;
class Block;
class KeyEntitlements;
class Policy;
class Schema;
class TransactionMaker;
class TransactionMakerSignature;

//================================================================//
// AccountKey
//================================================================//
class AccountKey {
public:
    
    shared_ptr < Account >              mAccount;
    const KeyAndPolicy*                 mKeyAndPolicy;
//    shared_ptr < PathEntitlement >      mEntitlements;
    
    //----------------------------------------------------------------//
    operator bool () const {
        return ( this->mAccount && this->mKeyAndPolicy );
    }
};

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
    static constexpr const u64 DEFAULT_BLOCK_SIZE   = 100;

    typedef SerializableMap < string, string > MinerURLMap;

    //----------------------------------------------------------------//
    bool                                affirmKey                   ( string accountName, string makerKeyName, string keyName, const CryptoKey& key, const Policy* policy );
    bool                                awardAsset                  ( const Schema& schema, string accountName, string assetType, size_t quantity );
    bool                                deleteKey                   ( string accountName, string keyName );
    shared_ptr < Account >              getAccount                  ( Account::Index index ) const;
    shared_ptr < Account >              getAccount                  ( string accountName ) const;
    Account::Index                      getAccountIndex             ( string accountName ) const;
    AccountKey                          getAccountKey               ( string accountName, string keyName ) const;
    shared_ptr < AccountKeyLookup >     getAccountKeyLookup         ( string keyID ) const;
    string                              getAccountName              ( Account::Index accountIndex ) const;
    u64                                 getAccountNonce             ( Account::Index accountIndex ) const;
    shared_ptr < Asset >                getAsset                    ( const Schema& schema, Asset::Index index ) const;
    shared_ptr < Block >                getBlock                    () const;
    shared_ptr < Block >                getBlock                    ( size_t height ) const;
    u64                                 getBlockSize                () const;
    Entropy                             getEntropy                  () const;
    string                              getIdentity                 () const;
    SerializableList < Asset >          getInventory                ( const Schema& schema, string accountName ) const;
    shared_ptr < MinerInfo >            getMinerInfo                ( string accountName ) const;
    map < string, MinerInfo >           getMiners                   () const;
    shared_ptr < MinerURLMap >          getMinerURLs                () const;
    void                                getSchema                   ( Schema& schema ) const;
    string                              getSchemaString             () const;
    string                              getTransactionNote          ( string accountName, u64 nonce ) const;
    UnfinishedBlockList                 getUnfinished               ();
    void                                incrementNonce              ( Account::Index index, u64 nonce, string note );
    void                                init                        ();
    bool                                invoke                      ( const Schema& schema, string accountName, const AssetMethodInvocation& invocation );
    static bool                         isAccountName               ( string accountName );
    static bool                         isChildName                 ( string accountName );
    static bool                         isSuffix                    ( string suffix );
    bool                                isGenesis                   () const;
                                        Ledger                      ();
                                        Ledger                      ( Ledger& other );
                                        ~Ledger                     ();
    bool                                newAccount                  ( string accountName, u64 balance, string keyName, const CryptoKey& key, const Policy& keyPolicy, const Policy& accountPolicy );
    bool                                publishSchema               ( const Schema& schema );
    bool                                registerMiner               ( string accountName, string keyName, string url );
    void                                setAccount                  ( const Account& account );
    void                                setAccountEntitlements      ( string name, const Entitlements& entitlements );
    bool                                setAssetFieldValue          ( const Schema& schema, Asset::Index index, string fieldName, const AssetFieldValue& field );
    void                                setBlock                    ( const Block& block );
    void                                setEntitlements             ( string name, const Entitlements& entitlements );
    void                                setEntropyString            ( string entropy );
    bool                                setIdentity                 ( string identity );
    void                                serializeEntitlements       ( const Account& account, AbstractSerializerTo& serializer ) const;
    void                                setUnfinished               ( const UnfinishedBlockList& unfinished );
    bool                                verify                      ( const Schema& schema, const AssetMethodInvocation& invocation ) const;

    //----------------------------------------------------------------//
    template < typename ENTITLEMENTS_FAMILY >
    Entitlements getEntitlements ( string name ) const {
        
        if ( name.size () == 0 ) {
            return *ENTITLEMENTS_FAMILY::getMasterEntitlements ();
        }
        LedgerKey KEY_FOR_ENTITLEMENTS = FormatLedgerKey::forEntitlements ( name );
        shared_ptr < Entitlements > entitlements = this->getObjectOrNull < Entitlements >( KEY_FOR_ENTITLEMENTS );
        return entitlements ? *entitlements : Entitlements ();
    }

    //----------------------------------------------------------------//
    template < typename ENTITLEMENTS_FAMILY >
    Entitlements getEntitlements ( const Policy& policy ) const {
        
        Entitlements entitlements = this->getEntitlements < ENTITLEMENTS_FAMILY >( policy.getBase ());
        return *policy.applyRestrictions ( entitlements );
    }

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
    template < typename ENTITLEMENTS_FAMILY >
    bool isMoreRestrictivePolicy ( const Policy& policy, const Policy& restriction ) const {

        Entitlements entitlements = this->getEntitlements < ENTITLEMENTS_FAMILY >( policy );
        Entitlements restrictionEntitlements = this->getEntitlements < ENTITLEMENTS_FAMILY >( restriction );
        
        return entitlements.isMatchOrSubsetOf ( &restrictionEntitlements );
    }
    
    //----------------------------------------------------------------//
    template < typename ENTITLEMENTS_FAMILY >
    bool isValidPolicy ( const Policy& policy ) const {

        Entitlements entitlements = this->getEntitlements < ENTITLEMENTS_FAMILY >( policy.getBase ());
        return policy.isMatchOrSubsetOf ( entitlements );
    }
    
    //----------------------------------------------------------------//
    template < typename ENTITLEMENTS_FAMILY >
    bool isValidPolicy ( const Policy& policy, const Policy& restriction ) const {

        Entitlements entitlements = this->getEntitlements < ENTITLEMENTS_FAMILY >( policy.getBase ());
        if ( !policy.isMatchOrSubsetOf ( entitlements )) return false;
        entitlements = *policy.applyRestrictions ( entitlements );
        
        Entitlements restrictionEntitlements = this->getEntitlements < ENTITLEMENTS_FAMILY >( restriction );
        return entitlements.isMatchOrSubsetOf ( &restrictionEntitlements );
    }
    
    //----------------------------------------------------------------//
    template < typename ENTITLEMENTS_FAMILY >
    const Policy* resolveBequest ( const Policy& sponsor, const Policy* bequest, const Policy* proposed ) const {

        const Policy* selection = bequest ? bequest : &sponsor;
        if ( proposed ) {
            if ( !this->isValidPolicy < ENTITLEMENTS_FAMILY >( *proposed, *selection )) return NULL;
            return proposed;
        }
        return selection;
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
