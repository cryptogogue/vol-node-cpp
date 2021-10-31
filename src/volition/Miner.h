// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_MINER_H
#define VOLITION_MINER_H

#include <volition/common.h>
#include <volition/AbstractMiningMessenger.h>
#include <volition/Accessors.h>
#include <volition/AbstractBlockTree.h>
#include <volition/BlockTreeSampler.h>
#include <volition/PayoutPolicy.h>
#include <volition/TransactionFeeSchedule.h>
#include <volition/CryptoKey.h>
#include <volition/Ledger.h>
#include <volition/MonetaryPolicy.h>
#include <volition/RemoteMiner.h>
#include <volition/TransactionQueue.h>
#include <volition/serialization/AbstractSerializable.h>
#include <volition/Singleton.h>
#include <volition/TransactionStatus.h>

namespace Volition {

class AbstractChainRecorder;
class AbstractHashable;
class BlockSearchPool;
class Miner;

//================================================================//
// MinerConfig
//================================================================//
class MinerConfig :
    public AbstractSerializable {
public:

    string      mReward;
    u64         mMinimumGratuity;
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
    
        serializer.serialize ( "reward",            this->mReward );
        serializer.serialize ( "minimumGratuity",   this->mMinimumGratuity );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
    
        serializer.serialize ( "reward",            this->mReward );
        serializer.serialize ( "minimumGratuity",   this->mMinimumGratuity );
    }
    
    //----------------------------------------------------------------//
    MinerConfig () :
        mMinimumGratuity ( 0 ) {
    }
};

//================================================================//
// MinerSnapshot
//================================================================//
class MinerSnapshot {
public:

    string                              mMinerID;
    time_t                              mStartTime;

    CryptoKeyPair                       mKeyPair;
    string                              mMotto;
    Signature                           mVisage;
    bool                                mIsMiner;
    u64                                 mMinerHeight;
    
    string                              mPrefixFilename;
    string                              mLedgerFilename;
    string                              mConfigFilename;
    string                              mBlocksFilename;
    string                              mMinersFilename;

    set < string >                      mOnlineMinerURLs;
    
    GET ( time_t,                       StartTime,                  mStartTime )
    GET ( const Signature&,             Visage,                     mVisage )
    GET_SET ( const CryptoKeyPair&,     KeyPair,                    mKeyPair )
    GET_SET ( string,                   MinerID,                    mMinerID )
    GET_SET ( string,                   Motto,                      mMotto )
    
    //----------------------------------------------------------------//
                            MinerSnapshot                   ();
    set < string >          sampleOnlineMinerURLs           ( size_t sampleSize = 0 ) const;
};

//================================================================//
// MinerStatus
//================================================================//
// TODO: this is a hack to speed up the default node endpoint.
class MinerStatus {
public:

    SchemaVersion               mSchemaVersion;
    string                      mSchemaHash;
    string                      mGenesisHash;
    string                      mIdentity;
    u64                         mMinimumGratuity;
    string                      mReward;
    u64                         mTotalBlocks;
    TransactionFeeSchedule      mFeeSchedule;
    MonetaryPolicy              mMonetaryPolicy;
    PayoutPolicy                mPayoutPolicy;
    u64                         mMinerBlockCount;
    
    u64                         mRewardPool;
    u64                         mPrizePool;
    u64                         mPayoutPool;
    u64                         mVOL;
    
    u64                         mAcceptedRelease;
};

//================================================================//
// Miner
//================================================================//
class Miner :
    public AbstractSerializable,
    public AbstractMiningMessengerClient,
    public MinerSnapshot {
public:

    enum Control {
        CONTROL_NONE,
        CONTROL_CONFIG,
        CONTROL_ADMIN,
    };

    enum ReportMode {
        REPORT_NONE,
        REPORT_BEST_BRANCH,
        REPORT_ALL_BRANCHES,
    };

    enum : int {
        MINER_VERBOSE               = 0x02,
        MINER_MUTE                  = 0x08,
    };

    static const int DEFAULT_FLAGS = 0;

protected:

    friend class AbstractChainRecorder;
    friend class BlockSearch;
    friend class RemoteMiner;

    friend class ScopedExclusiveMinerLock;
    friend class ScopedSharedMinerLock;
    friend class ScopedUniqueMinerBlockTreeLock;
    friend class ScopedSharedMinerLedgerLock;

    static constexpr const char* PERSIST_PREFIX     = "v5-beta-";
    static constexpr const char* MASTER_BRANCH      = "master";

    int                                             mFlags;
    bool                                            mNeedsReport;
    ReportMode                                      mReportMode;
    Block::VerificationPolicy                       mBlockVerificationPolicy;

    CryptoPublicKey                                 mControlKey;
    Control                                         mControlLevel;

    MinerConfig                                     mConfig;
    
    set < string >                                  mNewMinerURLs;
    set < string >                                  mCompletedURLs;
    
    set < shared_ptr < RemoteMiner >>               mRemoteMiners;
    map < string, shared_ptr < RemoteMiner >>       mRemoteMinersByID;
    map < string, shared_ptr < RemoteMiner >>       mRemoteMinersByURL;
    
    set < shared_ptr < RemoteMiner >>               mOnlineMiners;
    set < shared_ptr < RemoteMiner >>               mContributors;
    
    bool                                            mNetworkSearch;
    
    shared_ptr < AbstractBlockTree >                mBlockTree;
    mutex                                           mBlockTreeMutex;
    shared_ptr < BlockSearchPool >                  mBlockSearchPool;
    
    // the "working" legder is a "complete" chain (in that all blocks are accounted for),
    // but still likely to revert if a superior branch is discovered.
    shared_ptr < Ledger >                           mLedger;
    BlockTreeTag                                    mLedgerTag;
    
    // the "provisional" branch may not be complete and may end with a "provisional"
    // block header. the "best" provisional branch is the branch the miner is working
    // to complete by gathering (or building) the missing blocks. the provisional
    // branch is not published to other miners.
    BlockTreeTag                                    mBestBranchTag;
    
    // this is the persistence provider for the ledger. it serves two purposes:
    // to offload the RAM burder required to persist the ledger database and to speed
    // up rebuilding the database when restarting the node.
    shared_ptr < AbstractPersistenceProvider >      mPersistenceProvider;
    size_t                                          mPersistFrequency;
    size_t                                          mRetryPersistenceCheck;
    size_t                                          mPersistenceSleep;
    
    mutex                                           mMutex;
    
    MinerSnapshot                                   mSnapshot;
    MinerStatus                                     mMinerStatus;
    shared_mutex                                    mSnapshotMutex;
    
    LockedLedger                                    mLockedLedger;
    shared_mutex                                    mLockedLedgerMutex;
    
    shared_ptr < AbstractMiningMessenger >          mMessenger;
    shared_ptr < TransactionQueue >                 mTransactionQueue;
    
    u64                                             mAcceptedRelease; // will accept blocks with this release
    u64                                             mProducedRelease; // will produce blocks with this release
    
    //----------------------------------------------------------------//
    void                                affirmMessenger             ();
    bool                                checkTags                   () const;
    void                                composeChain                ( BlockTreeCursor cursor );
    void                                composeChainInnerLoop       ( BlockTreeCursor branch );
    u64                                 findRelease                 () const;
    BlockTreeCursor                     improveBranch               ( BlockTreeCursor tail, u64 consensusHeight, time_t now );
    LedgerResult                        persistLedger               ( shared_ptr < AbstractPersistenceProvider > provider, shared_ptr < const Block > genesisBlock );
    shared_ptr < BlockHeader >          prepareProvisional          ( const BlockHeader& parent, time_t now ) const;
    void                                pushBlock                   ( shared_ptr < const Block > block );
    set < shared_ptr < RemoteMiner >>   sampleContributors          ( size_t sampleSize ) const;
    set < shared_ptr < RemoteMiner >>   sampleOnlineMiners          ( size_t sampleSize ) const;
    void                                saveChain                   ();
    void                                saveConfig                  ();
    void                                scheduleReport              ();
    void                                updateBestBranch            ( time_t now );
    void                                updateBlockSearches         ();
    void                                updateMinerStatus           ();
    void                                updateNetworkSearches       ();
    void                                updateRelease               ();
    void                                updateRemoteMinerGroups     ();
    void                                updateRemoteMiners          ();

    //----------------------------------------------------------------//
    void                                AbstractMiningMessengerClient_receiveResponse   ( const MiningMessengerResponse& response, time_t now ) override;
    void                                AbstractSerializable_serializeFrom              ( const AbstractSerializerFrom& serializer ) override;
    void                                AbstractSerializable_serializeTo                ( AbstractSerializerTo& serializer ) const override;
    virtual void                        Miner_reset                                     ();
    virtual void                        Miner_shutdown                                  ( bool kill );

public:

    enum class SubmissionResponse {
        ACCEPTED = 0,
        RESUBMIT_EARLIER,
    };
    
    GET ( BlockTreeCursor,                                  BestProvisional,            mBestBranchTag.getCursor ())
    GET ( const AbstractBlockTree&,                         BlockTree,                  *mBlockTree )
    GET ( const Ledger&,                                    Ledger,                     *mLedger )
    GET ( BlockTreeCursor,                                  LedgerTag,                  mLedgerTag.getCursor ())
    GET ( const LockedLedger&,                              LockedLedger,               mLockedLedger )
    GET ( u64,                                              MinimumGratuity,            mConfig.mMinimumGratuity )
    GET ( string,                                           Reward,                     mConfig.mReward )
    GET ( TransactionQueue&,                                TransactionQueue,           *mTransactionQueue )
        
    GET_SET ( const CryptoPublicKey&,                       ControlKey,                 mControlKey )
    GET_SET ( Control,                                      ControlLevel,               mControlLevel )
    GET_SET ( shared_ptr < AbstractMiningMessenger >,       Messenger,                  mMessenger )
    GET_SET ( shared_ptr < AbstractPersistenceProvider >,   PersistenceProvider,        mPersistenceProvider )
    GET_SET ( size_t,                                       PersistFrequency,           mPersistFrequency )
    GET_SET ( ReportMode,                                   ReportMode,                 mReportMode )
    GET_SET ( size_t,                                       RetryPersistenceCheck,      mRetryPersistenceCheck )
    GET_SET ( size_t,                                       PersistenceSleep,           mPersistenceSleep )

    //----------------------------------------------------------------//
    void                            affirmKey                           ( uint keyLength = CryptoKeyPair::RSA_1024, unsigned long exp = CryptoKeyPair::RSA_EXP_65537 );
    void                            affirmRemoteMiner                   ( string url );
    void                            affirmVisage                        ();
    Signature                       calculateVisage                     ( string motto = "" );
    static Signature                calculateVisage                     ( const CryptoKeyPair& keyPair, string motto = "" );
    size_t                          getChainSize                        () const;
    Ledger&                         getLedger                           ();
    Ledger                          getLedgerAtBlock                    ( u64 index ) const;
    void                            getSnapshot                         ( MinerSnapshot* snapshot = NULL, MinerStatus* status = NULL );
    bool                            isLazy                              () const;
    static shared_ptr < Block >     loadGenesisBlock                    ( string genesisFile );
    void                            loadKey                             ( string keyfile, string password = "" );
                                    Miner                               ();
    virtual                         ~Miner                              ();
    LedgerResult                    persistBlockTreeSQLite              ( SQLiteConfig config );
    LedgerResult                    persistLedgerSQLite                 ( shared_ptr < const Block > genesisBlock, SQLiteConfig config );
    LedgerResult                    persistLedgerSQLiteStringStore      ( shared_ptr < const Block > genesisBlock, SQLiteConfig config );
    LedgerResult                    persistLedgerDebugStringStore       ( shared_ptr < const Block > genesisBlock );
    shared_ptr < Block >            prepareBlock                        ( time_t now );
    void                            report                              () const;
    void                            report                              ( ReportMode reportMode ) const;
    void                            reset                               ();
    void                            setBlockTree                        ( shared_ptr < AbstractBlockTree > blockTree = NULL );
    void                            setGenesis                          ( shared_ptr < const Block > block );
    void                            setMaxBlockSearches                 ( size_t max );
    void                            setMinimumGratuity                  ( u64 minimumGratuity );
    void                            setMute                             ( bool paused );
    void                            setPersistencePath                  ( string path, shared_ptr < const Block > genesisBlock );
    void                            setReward                           ( string reward );
    void                            setVerbose                          ( bool verbose = true );
    void                            shutdown                            ( bool kill = false );
    void                            step                                ( time_t now );
};

} // namespace Volition
#endif
