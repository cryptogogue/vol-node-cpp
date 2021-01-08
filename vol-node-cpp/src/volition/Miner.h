// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_MINER_H
#define VOLITION_MINER_H

#include <volition/common.h>
#include <volition/AbstractMiningMessenger.h>
#include <volition/Accessors.h>
#include <volition/AbstractBlockTree.h>
#include <volition/CryptoKey.h>
#include <volition/Ledger.h>
#include <volition/RemoteMiner.h>
#include <volition/TransactionQueue.h>
#include <volition/serialization/AbstractSerializable.h>
#include <volition/Singleton.h>
#include <volition/TransactionStatus.h>

namespace Volition {

class AbstractChainRecorder;
class AbstractHashable;
class Miner;

//================================================================//
// BlockSearch
//================================================================//
class BlockSearch {
protected:

    static const size_t SEARCH_SIZE = 16;

    friend class Miner;

    string              mHash;
    set < string >      mActiveMiners;
    set < string >      mCompletedMiners;

public:

    //----------------------------------------------------------------//
                    BlockSearch     ();
    bool            step            ( Miner& miner );
    void            step            ( shared_ptr < RemoteMiner > remoteMiner );
};

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
// Miner
//================================================================//
class Miner :
    public AbstractSerializable,
    public AbstractMiningMessengerClient,
    public TransactionQueue {
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

    static constexpr const char* MASTER_BRANCH      = "master";

    string                                          mMinerID;
    string                                          mURL;
    time_t                                          mStartTime;

    CryptoKeyPair                                   mKeyPair;
    string                                          mMotto;
    Signature                                       mVisage;

    int                                             mFlags;
    bool                                            mNeedsReport;
    ReportMode                                      mReportMode;
    kRewriteMode                                    mRewriteMode;
    Block::VerificationPolicy                       mBlockVerificationPolicy;

    string                                          mReward;
    u64                                             mMinimumGratuity;

    CryptoPublicKey                                 mControlKey;
    Control                                         mControlLevel;

    MinerConfig                                     mConfig;
    
    set < string >                                  mNewMinerURLs;
    set < string >                                  mActiveMinerURLs;
    map < string, shared_ptr < RemoteMiner >>       mRemoteMinersByID;
    map < string, shared_ptr < RemoteMiner >>       mRemoteMinersByURL;
    set < shared_ptr < RemoteMiner >>               mOnlineMiners;
    map < string, BlockSearch >                     mBlockSearches;
    set < string >                                  mHeaderSearches;
    bool                                            mNetworkSearch;
    
    shared_ptr < AbstractBlockTree >                mBlockTree;
    
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
    string                                          mLedgerFilename;
    string                                          mConfigFilename;
    string                                          mBlocksFilename;
    
    Poco::Mutex                                     mMutex;
    shared_ptr < AbstractMiningMessenger >          mMessenger;
    
    //----------------------------------------------------------------//
    void                                affirmBlockSearch           ( BlockTreeCursor cursor );
    void                                affirmBranchSearch          ( BlockTreeCursor cursor );
    void                                affirmMessenger             ();
    double                              checkConsensus              ( BlockTreeCursor tag ) const;
    void                                composeChain                ();
    void                                composeChainRecurse         ( BlockTreeCursor branch );
    void                                discoverMiners              ();
    BlockSearch*                        findBlockSearch             ( const Digest& digest );
    BlockTreeCursor                     improveBranch               ( BlockTreeTag& tag, BlockTreeCursor tail, time_t now );
    void                                pushBlock                   ( shared_ptr < const Block > block );
    void                                report                      () const;
    void                                saveChain                   ();
    void                                saveConfig                  ();
    void                                scheduleReport              ();
    void                                updateBestBranch            ( time_t now );
    void                                updateBlockSearches         ();
    void                                updateHeaderSearches        ();
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
    GET ( u64,                                              MinimumGratuity,            mConfig.mMinimumGratuity )
    GET ( string,                                           Reward,                     mConfig.mReward )
    GET ( time_t,                                           StartTime,                  mStartTime )
    GET ( const Signature&,                                 Visage,                     mVisage )
    
    SET ( shared_ptr < AbstractMiningMessenger >,           Messenger,                  mMessenger )
    
    GET_SET ( const CryptoPublicKey&,                       ControlKey,                 mControlKey )
    GET_SET ( Control,                                      ControlLevel,               mControlLevel )
    GET_SET ( const CryptoKeyPair&,                         KeyPair,                    mKeyPair )
    GET_SET ( string,                                       MinerID,                    mMinerID )
    GET_SET ( string,                                       Motto,                      mMotto )
    GET_SET ( ReportMode,                                   ReportMode,                 mReportMode )
    GET_SET ( kRewriteMode,                                 RewriteMode,                mRewriteMode)
    GET_SET ( string,                                       URL,                        mURL )
    GET_SET ( shared_ptr < AbstractPersistenceProvider >,   PersistenceProvider,        mPersistenceProvider )
    
    //----------------------------------------------------------------//
    operator Poco::Mutex& () {
    
        return this->mMutex;
    }

    //----------------------------------------------------------------//
    void                                affirmKey                   ( uint keyLength = CryptoKeyPair::RSA_1024, unsigned long exp = CryptoKeyPair::RSA_EXP_65537 );
    void                                affirmRemoteMiner           ( string url );
    void                                affirmVisage                ();
    static Signature                    calculateVisage             ( const CryptoKeyPair& keyPair, string motto = "" );
    bool                                checkBestBranch             ( string miners ) const;
    size_t                              countBranches               () const;
    void                                extend                      ( time_t now );
    const set < string >&               getActiveMinerURLs          () const;
    size_t                              getChainSize                () const;
    Ledger&                             getLedger                   ();
    TransactionStatus                   getTransactionStatus        ( string accountName, string uuid ) const;
    bool                                isLazy                      () const;
    static shared_ptr < Block >         loadGenesisBlock            ( string genesisFile );
    static shared_ptr < Block >         loadGenesisLedger           ( string genesisFile );
    void                                loadKey                     ( string keyfile, string password = "" );
                                        Miner                       ();
    virtual                             ~Miner                      ();
    void                                persist                     ( string path, shared_ptr < const Block > block );
    shared_ptr < Block >                prepareBlock                ( time_t now );
    shared_ptr < BlockHeader >          prepareProvisional          ( const BlockHeader& parent, time_t now ) const;
    void                                pruneTransactions           ();
    void                                reset                       ();
    set < string >                      sampleActiveMinerURLs       ( size_t sampleSize ) const;
    void                                setGenesis                  ( shared_ptr < const Block > block );
    void                                setMinimumGratuity          ( u64 minimumGratuity );
    void                                setMute                     ( bool paused );
    void                                setReward                   ( string reward );
    void                                setRewriteWindow            ();
    void                                setVerbose                  ( bool verbose = true );
    void                                shutdown                    ( bool kill = false );
    void                                step                        ( time_t now );
};

//================================================================//
// ScopedMinerLock
//================================================================//
class ScopedMinerLock {
private:

    shared_ptr < Miner >                mMiner;
    Poco::ScopedLock < Poco::Mutex >    mScopedLock;

public:

    //----------------------------------------------------------------//
    ScopedMinerLock ( shared_ptr < Miner > minerActivity ) :
        mMiner ( minerActivity ),
        mScopedLock ( *minerActivity ) {
    }
};

} // namespace Volition
#endif
