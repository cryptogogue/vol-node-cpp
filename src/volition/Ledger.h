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
class ContractWithDigest;
class KeyEntitlements;
class MonetaryPolicy;
class PayoutPolicy;
class Policy;
class Schema;
class TransactionFeeSchedule;
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
// AbstractLedger
//================================================================//
class AbstractLedger :
    public virtual AbstractVersionedStoreTag,
    public AbstractSerializable,
    virtual public AbstractLedgerComponent,
    public Ledger_Account,
    public Ledger_Dump,
    public Ledger_Inventory,
    public Ledger_Miner {
protected:

    friend class Ledger;
    friend class LockedLedger;
    friend class LockedLedgerIterator;

    //----------------------------------------------------------------//
    AbstractLedger&             AbstractLedgerComponent_getLedger           () override;
    const AbstractLedger&       AbstractLedgerComponent_getLedger           () const override;
    void                        AbstractSerializable_serializeFrom          ( const AbstractSerializerFrom& serializer ) override;
    void                        AbstractSerializable_serializeTo            ( AbstractSerializerTo& serializer ) const override;

public:

    mutable map < string, shared_ptr < const Schema >> mSchemaCache;

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
    static LedgerKey keyFor_blockHeightByHash ( string hash ) {
        return Format::write ( "blockHeightByHash.%s", hash.c_str ());
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
    static LedgerKey keyFor_globalAccountCount () {
        return "account.count";
    }

    //----------------------------------------------------------------//
    static LedgerKey keyFor_globalAssetCount () {
        return Format::write ( "asset.count" );
    }

    //----------------------------------------------------------------//
    static LedgerKey keyFor_identity () {
        return "identity";
    }

    //----------------------------------------------------------------//
    static LedgerKey keyFor_maxBlockWeight () {
        return "maxBlockWeight";
    }

    //----------------------------------------------------------------//
    static LedgerKey keyFor_miners () {
        return "miners";
    }
    
    //----------------------------------------------------------------//
    static LedgerKey keyFor_monetaryPolicy () {
        return "monetaryPolicy";
    }
    
    //----------------------------------------------------------------//
    static LedgerKey keyFor_payoutPolicy () {
        return "payoutPolicy";
    }
    
    //----------------------------------------------------------------//
    static LedgerKey keyFor_payoutPool () {
        return "payoutPool";
    }
    
    //----------------------------------------------------------------//
    static LedgerKey keyFor_prizePool () {
        return "prizePool";
    }
    
    //----------------------------------------------------------------//
    static LedgerKey keyFor_rewardCount ( string name ) {

        assert ( name.size () > 0 );
        return Format::write ( "rewards.%s.count", name.c_str ());
    }
    
    //----------------------------------------------------------------//
    static LedgerKey keyFor_rewardPool () {
        return "rewardPool";
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
    static LedgerKey keyFor_termsOfService () {
        return "termsOfService";
    }
    
    //----------------------------------------------------------------//
    static LedgerKey keyFor_transactionFeeSchedule () {
        return "transactionFeeSchedule";
    }
    
    //----------------------------------------------------------------//
    static LedgerKey keyFor_unfinished () {
        return "unfinished";
    }

    //----------------------------------------------------------------//
    static LedgerKey keyFor_totalVOL () {
        return "totalVOL";
    }

    //----------------------------------------------------------------//
                                        AbstractLedger                  ();
                                        ~AbstractLedger                 ();
    bool                                canReward                       ( string rewardName ) const;
    bool                                checkMiners                     ( string miners ) const;
    LedgerResult                        checkSchemaMethodsAndRewards    ( const Schema& schema ) const;
    string                              chooseReward                    ( string rewardName );
    void                                clearSchemaCache                ();
    u64                                 countBlocks                     () const;
    u64                                 countVOL                        () const;
    u64                                 createVOL                       ( u64 rewards, u64 prizes );
    shared_ptr < const Block >          getBlock                        () const;
    shared_ptr < const Block >          getBlock                        ( u64 height ) const;
    shared_ptr < const Block >          getBlock                        ( string hash ) const;
    time_t                              getBlockDelayInSeconds          () const;
    Entropy                             getEntropy                      () const;
    string                              getEntropyString                () const;
    string                              getGenesisHash                  () const;
    shared_ptr < const BlockHeader >    getHeader                       ( u64 height ) const;
    u64                                 getHeight                       () const;
    string                              getIdentity                     () const;
    u64                                 getMaxBlockWeight               () const;
    MonetaryPolicy                      getMonetaryPolicy               () const;
    PayoutPolicy                        getPayoutPolicy                 () const;
    u64                                 getPayoutPool                   () const;
    u64                                 getPrizePool                    () const;
    u64                                 getRewardPool                   () const;
    time_t                              getRewriteWindowInSeconds       () const;
    const Schema&                       getSchema                       () const;
    SchemaVersion                       getSchemaVersion                () const;
    string                              getSchemaHash                   () const;
    string                              getSchemaString                 () const;
    ContractWithDigest                  getTermsOfService               () const;
    TransactionFeeSchedule              getTransactionFeeSchedule       () const;
    UnfinishedBlockList                 getUnfinished                   ();
    bool                                hasBlock                        ( string hash ) const;
    bool                                hasTransaction                  ( string accountName, string uuid ) const;
    void                                init                            ();
    LedgerResult                        invoke                          ( string accountName, const AssetMethodInvocation& invocation, time_t time );
    LedgerResult                        invokeReward                    ( string minerID, string rewardName, time_t time );
    bool                                isGenesis                       () const;
    void                                payout                          ( u64 amount );
    string                              printChain                      ( const char* pre = NULL, const char* post = NULL ) const;
    LedgerResult                        pushBlock                       ( const Block& block, Block::VerificationPolicy policy );
    void                                serializeEntitlements           ( const Account& account, AbstractSerializerTo& serializer ) const;
    void                                setEntitlements                 ( string name, const Entitlements& entitlements );
    void                                setEntropyString                ( string entropy );
    bool                                setIdentity                     ( string identity );
    void                                setMonetaryPolicy               ( const MonetaryPolicy& monetaryPolicy );
    LedgerResult                        setPayoutPolicy                 ( const PayoutPolicy& distributionTable );
    void                                setPayoutPool                   ( u64 pool );
    void                                setSchema                       ( const Schema& schema );
    void                                setTermsOfService               ( const ContractWithDigest& contract );
    void                                setTransactionFeeSchedule       ( const TransactionFeeSchedule& feeSchedule );
    void                                setUnfinished                   ( const UnfinishedBlockList& unfinished );
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
    void getObject ( LedgerKey key, TYPE& object ) const {
    
        AbstractLedger::getObject < TYPE >( *this, key, object );
    }

    //----------------------------------------------------------------//
    template < typename TYPE >
    static void getObject ( const AbstractHasVersionedBranch& snapshot, LedgerKey key, TYPE& object ) {
    
        string json = snapshot.getValueOrFallback < string >( key, "" );
        if ( json.size () > 0 ) {
            FromJSONSerializer::fromJSONString ( object, json );
        }
    }

    //----------------------------------------------------------------//
    template < typename TYPE >
    shared_ptr < TYPE > getObjectOrNull ( LedgerKey key ) const {

        return AbstractLedger::getObjectOrNull < TYPE >( *this, key );
    }
    
    //----------------------------------------------------------------//
    template < typename TYPE >
    static shared_ptr < TYPE > getObjectOrNull ( const AbstractHasVersionedBranch& snapshot, LedgerKey key ) {
    
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

//================================================================//
// Ledger
//================================================================//
class Ledger :
    public AbstractLedger,
    public VersionedStoreTag,
    public enable_shared_from_this < Ledger > {
public:

    //----------------------------------------------------------------//
    Ledger () {
    }
    
    //----------------------------------------------------------------//
    Ledger ( AbstractLedger& other ) :
        VersionedStoreTag ( other.getTag ()),
        enable_shared_from_this < Ledger > () {
        
        this->mSchemaCache = other.mSchemaCache;
    }
    
    //----------------------------------------------------------------//
    Ledger ( VersionedStoreTag& tag ) :
        VersionedStoreTag ( tag ),
        enable_shared_from_this < Ledger > () {
    }
};

//================================================================//
// LockedLedger
//================================================================//
class LockedLedger :
    public AbstractLedger,
    public VersionedStoreLock {
protected:

    //----------------------------------------------------------------//
    VersionedStoreTag& AbstractVersionedStoreTag_getTag () override {
        assert ( false );
        static VersionedStoreTag dummy;
        return dummy;
    }

public:

    //----------------------------------------------------------------//
    LockedLedger () {
    }

    //----------------------------------------------------------------//
    LockedLedger ( const AbstractLedger& other ) :
        VersionedStoreLock ( other ) {
        this->mSchemaCache = other.mSchemaCache;
    }
};

//================================================================//
// LockedLedgerIterator
//================================================================//
class LockedLedgerIterator :
    public AbstractLedger,
    public VersionedStoreIterator {
protected:

    //----------------------------------------------------------------//
    VersionedStoreTag& AbstractVersionedStoreTag_getTag () override {
        assert ( false );
        static VersionedStoreTag dummy;
        return dummy;
    }

public:
    
    //----------------------------------------------------------------//
    LockedLedgerIterator ( const AbstractLedger& other ) :
        VersionedStoreIterator ( other ) {
        this->mSchemaCache = other.mSchemaCache;
    }
};

} // namespace Volition
#endif
