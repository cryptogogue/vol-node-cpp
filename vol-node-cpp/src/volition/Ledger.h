// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_LEDGER_H
#define VOLITION_LEDGER_H

#include <volition/common.h>
#include <volition/AbstractLedgerComponent.h>
#include <volition/Account.h>
#include <volition/AccountEntitlements.h>
#include <volition/AccountKeyLookup.h>
#include <volition/Asset.h>
#include <volition/Block.h>
#include <volition/Entropy.h>
#include <volition/KeyEntitlements.h>
#include <volition/Ledger_Account.h>
#include <volition/Ledger_Dump.h>
#include <volition/Ledger_Inventory.h>
#include <volition/Ledger_Miner.h>
#include <volition/LedgerKey.h>
#include <volition/SchemaVersion.h>
#include <volition/serialization/Serialization.h>

namespace Volition {

class AccountEntitlements;
class AssetFieldValue;
class AssetMethod;
class AssetMethodInvocation;
class Block;
class FeeDistributionTable;
class FeeSchedule;
class KeyEntitlements;
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
    public AbstractSerializable,
    public VersionedStore,
    virtual public AbstractLedgerComponent,
    public Ledger_Account,
    public Ledger_Dump,
    public Ledger_Inventory,
    public Ledger_Miner {
private:

    shared_ptr < map < string, Schema >> mSchemaCache;

    //----------------------------------------------------------------//
    Ledger&             AbstractLedgerComponent_getLedger       () override;
    const Ledger&       AbstractLedgerComponent_getLedger       () const override;
    void                AbstractSerializable_serializeFrom      ( const AbstractSerializerFrom& serializer ) override;
    void                AbstractSerializable_serializeTo        ( AbstractSerializerTo& serializer ) const override;

public:

    static constexpr const u64 DEFAULT_BLOCK_SIZE_IN_POINTS             = 1024;     // not enforced for genesis block
    static constexpr const u64 DEFAULT_BLOCK_DELAY_IN_SECONDS           = 30;       // 0.5 min
    static constexpr const u64 DEFAULT_REWRITE_WINDOW_IN_SECONDS        = 600;      // 10 min

    //----------------------------------------------------------------//
    static LedgerKey keyFor_accountAlias ( string accountName ) {
        assert ( accountName.size () > 0 );
        return Format::write ( "accountAlias.%s", accountName.c_str ());
    }

    //----------------------------------------------------------------//
    static LedgerKey keyFor_accountKeyLookup ( string keyID ) {
        return Format::write ( "key.%s", keyID.c_str ());
    }
    
    //----------------------------------------------------------------//
    static LedgerKey keyFor_blockDelay () {
        return "blockDelay";
    }
    
    //----------------------------------------------------------------//
    static LedgerKey keyFor_blockSize () {
        return "blockSize";
    }

    //----------------------------------------------------------------//
    static LedgerKey keyFor_entitlements ( string name ) {

        assert ( name.size () > 0 );
        return Format::write ( "entitlements.%s", name.c_str ());
    }

    //----------------------------------------------------------------//
    static LedgerKey keyFor_entropy () {
        return "entropy";
    }
    
    //----------------------------------------------------------------//
    static LedgerKey keyFor_feeDistributionPool () {
        return "feeDistributionPool";
    }
    
    //----------------------------------------------------------------//
    static LedgerKey keyFor_feeDistributionTable () {
        return "feeDistributionTable";
    }
    
    //----------------------------------------------------------------//
    static LedgerKey keyFor_feeSchedule () {
        return "feeSchedule";
    }

    //----------------------------------------------------------------//
    static LedgerKey keyFor_globalAccountCount () {
        return "account.count";
    }

    //----------------------------------------------------------------//
    static LedgerKey keyFor_globalAssetCount () {
        return Format::write ( "asset.count" );
    }

    //----------------------------------------------------------------//
    static LedgerKey keyFor_globalBlockCount () {
        return "block.count";
    }

    //----------------------------------------------------------------//
    static LedgerKey keyFor_identity () {
        return "identity";
    }

    //----------------------------------------------------------------//
    static LedgerKey keyFor_miners () {
        return "miners";
    }
    
    //----------------------------------------------------------------//
    static LedgerKey keyFor_rewardCount ( string name ) {

        assert ( name.size () > 0 );
        return Format::write ( "rewards.%s.count", name.c_str ());
    }
    
    //----------------------------------------------------------------//
    static LedgerKey keyFor_rewriteWindow () {
        return "rewriteWindow";
    }
    
    //----------------------------------------------------------------//
    static LedgerKey keyFor_schema () {
        return "schema";
    }
    
    //----------------------------------------------------------------//
    static LedgerKey keyFor_schemaVersion () {
        return "schemaVersion";
    }
    
    //----------------------------------------------------------------//
    static LedgerKey keyFor_schemaHash () {
        return "schemaHash";
    }
    
    //----------------------------------------------------------------//
    static LedgerKey keyFor_unfinished () {
        return "unfinished";
    }

    //----------------------------------------------------------------//
    static LedgerKey keyFor_VOL () {
        return "VOL";
    }

    //----------------------------------------------------------------//
    bool                                checkMiners                     ( string miners ) const;
    LedgerResult                        checkSchemaMethodsAndRewards    ( const Schema& schema ) const;
    void                                clearSchemaCache                ();
    size_t                              countBlocks                     () const;
    u64                                 countVOL                        () const;
    u64                                 createVOL                       ( u64 amount );
    void                                distribute                      ( u64 amount );
    shared_ptr < const Block >          getBlock                        () const;
    shared_ptr < const Block >          getBlock                        ( u64 height ) const;
    time_t                              getBlockDelayInSeconds          () const;
    u64                                 getBlockSizeInPoints            () const;
    Entropy                             getEntropy                      () const;
    string                              getEntropyString                () const;
    u64                                 getFeeDistributionPool          () const;
    FeeSchedule                         getFeeSchedule                  () const;
    string                              getGenesisHash                  () const;
    u64                                 getHeight                       () const;
    string                              getIdentity                     () const;
    time_t                              getRewriteWindowInSeconds       () const;
    const Schema&                       getSchema                       ();
    SchemaVersion                       getSchemaVersion                () const;
    string                              getSchemaHash                   () const;
    string                              getSchemaString                 () const;
    UnfinishedBlockList                 getUnfinished                   ();
    bool                                hasTransaction                  ( string accountName, string uuid ) const;
    void                                init                            ();
    LedgerResult                        invoke                          ( string accountName, const AssetMethodInvocation& invocation, time_t time );
    LedgerResult                        invokeReward                    ( string minerID, string rewardName, time_t time );
    bool                                isGenesis                       () const;
                                        Ledger                          ();
                                        Ledger                          ( Ledger& other );
                                        ~Ledger                         ();
    string                              printChain                      ( const char* pre = NULL, const char* post = NULL ) const;
    bool                                pushBlock                       ( const Block& block, Block::VerificationPolicy policy );
    void                                serializeEntitlements           ( const Account& account, AbstractSerializerTo& serializer ) const;
    void                                setEntitlements                 ( string name, const Entitlements& entitlements );
    void                                setEntropyString                ( string entropy );
    void                                setFeeDistributionPool          ( u64 pool );
    LedgerResult                        setFeeDistributionTable         ( const FeeDistributionTable& distributionTable );
    void                                setFeeSchedule                  ( const FeeSchedule& feeSchedule );
    bool                                setIdentity                     ( string identity );
    void                                setSchema                       ( const Schema& schema );
    void                                setUnfinished                   ( const UnfinishedBlockList& unfinished );
    void                                setVOL                          ( u64 vol );
    bool                                verify                          ( const AssetMethodInvocation& invocation );

    //----------------------------------------------------------------//
    template < typename ENTITLEMENTS_FAMILY >
    Entitlements getEntitlements ( string name ) const {
        
        if ( name.size () == 0 ) {
            return *ENTITLEMENTS_FAMILY::getMasterEntitlements ();
        }
        LedgerKey KEY_FOR_ENTITLEMENTS = keyFor_entitlements ( name );
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
