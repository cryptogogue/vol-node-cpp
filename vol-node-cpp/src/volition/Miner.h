// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_MINER_H
#define VOLITION_MINER_H

#include <volition/common.h>
#include <volition/AbstractMiningMessenger.h>
#include <volition/Accessors.h>
#include <volition/BlockTree.h>
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

    friend class Miner;

    BlockTreeNode::ConstPtr         mSearchTarget;
    set < string >                  mCompletedMiners;
    shared_ptr < RemoteMiner >      mCurrentSearch;
    
    //----------------------------------------------------------------//
    void            step            ( Miner& miner );
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
    BlockTreeNode::RewriteMode                      mRewriteMode;
    Block::VerificationPolicy                       mBlockVerificationPolicy;

    string                                          mReward;
    u64                                             mMinimumGratuity;

    CryptoPublicKey                                 mControlKey;
    Control                                         mControlLevel;

    MinerConfig                                     mConfig;

    shared_ptr < AbstractChainRecorder >            mChainRecorder;
    
    set < string >                                  mNewMinerURLs;
    set < string >                                  mActiveMinerURLs;
    map < string, shared_ptr < RemoteMiner >>       mRemoteMinersByID;
    map < string, shared_ptr < RemoteMiner >>       mRemoteMinersByURL;
    set < shared_ptr < RemoteMiner >>               mOnlineMiners;
    map < string, BlockSearch >                     mBlockSearches;
    set < string >                                  mHeaderSearches;
    
    BlockTree                                       mBlockTree;
    
    shared_ptr < Ledger >                           mChain;                 // may run behind block tree tag
    Ledger                                          mHighConfidenceLedger;  // may run behind main ledger
    
    BlockTreeNode::ConstPtr                         mChainTag;              // node corresponding to top of chain
    BlockTreeNode::ConstPtr                         mHighConfidenceTag;     // node corresponding to top of chain
    BlockTreeNode::ConstPtr                         mBestBranch;            // "leaf" of the current chain
    list < BlockTreeNode::ConstPtr >                mNodeQueue;
    
    Poco::Mutex                                     mMutex;

    shared_ptr < AbstractMiningMessenger >          mMessenger;
    
    //----------------------------------------------------------------//
    void                                affirmBlockSearch           ( BlockTreeNode::ConstPtr node );
    void                                affirmBranchSearch          ( BlockTreeNode::ConstPtr node );
    void                                affirmMessenger             ();
    bool                                canExtend                   ( time_t now ) const;
    double                              checkConsensus              ( BlockTreeNode::ConstPtr tag ) const;
    void                                composeChain                ();
    void                                discoverMiners              ();
    BlockSearch*                        findBlockSearch             ( const Digest& digest );
    void                                pushBlock                   ( shared_ptr < const Block > block );
    void                                report                      () const;
    void                                saveChain                   ();
    void                                saveConfig                  ();
    void                                scheduleReport              ();
    void                                selectBestBranch            ( time_t now );
    BlockTreeNode::ConstPtr             truncate                    ( BlockTreeNode::ConstPtr tail, time_t now ) const;
    void                                updateChainRecurse          ( BlockTreeNode::ConstPtr branch );
    void                                updateBlockSearches         ( time_t now );
    void                                updateHeaderSearches        ();
    void                                updateHighConfidenceTag     ();
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
    
    GET ( BlockTreeNode::ConstPtr,                  BestBranch,                 mBestBranch )
    GET ( const BlockTree&,                         BlockTree,                  mBlockTree )
    GET ( BlockTreeNode::ConstPtr,                  ChainTag,                   mChainTag )
    GET ( const Ledger&,                            Ledger,                     mHighConfidenceLedger )
    GET ( u64,                                      MinimumGratuity,            mConfig.mMinimumGratuity )
    GET ( string,                                   Reward,                     mConfig.mReward )
    GET ( time_t,                                   StartTime,                  mStartTime )
    GET ( const Signature&,                         Visage,                     mVisage )
    GET ( const Ledger&,                            WorkingLedger,              *mChain )
    
    SET ( shared_ptr < AbstractMiningMessenger >,   Messenger,                  mMessenger )
    
    GET_SET ( const CryptoPublicKey&,               ControlKey,                 mControlKey )
    GET_SET ( Control,                              ControlLevel,               mControlLevel )
    GET_SET ( const CryptoKeyPair&,                 KeyPair,                    mKeyPair )
    GET_SET ( string,                               MinerID,                    mMinerID )
    GET_SET ( string,                               Motto,                      mMotto )
    GET_SET ( ReportMode,                           ReportMode,                 mReportMode )
    GET_SET ( BlockTreeNode::RewriteMode,           RewriteMode,                mRewriteMode)
    GET_SET ( string,                               URL,                        mURL )
    
    //----------------------------------------------------------------//
    operator Poco::Mutex& () {
    
        return this->mMutex;
    }

    //----------------------------------------------------------------//
    void                                affirmKey                   ( uint keyLength = CryptoKeyPair::RSA_1024, unsigned long exp = CryptoKeyPair::RSA_EXP_65537 );
    void                                affirmRemoteMiner           ( string url );
    void                                affirmVisage                ();
    bool                                checkBestBranch             ( string miners ) const;
    size_t                              countBranches               () const;
    void                                extend                      ( time_t now );
    const set < string >&               getActiveMinerURLs          () const;
    size_t                              getChainSize                () const;
    Ledger&                             getLedger                   ();
    TransactionStatus                   getTransactionStatus        ( string accountName, string uuid ) const;
    Ledger&                             getWorkingLedger            ();
    bool                                isLazy                      () const;
    void                                loadGenesisBlock            ( string path );
    void                                loadGenesisLedger           ( string path );
    void                                loadKey                     ( string keyfile, string password = "" );
                                        Miner                       ();
    virtual                             ~Miner                      ();
    shared_ptr < Block >                prepareBlock                ( time_t now );
    void                                reset                       ();
    set < string >                      sampleActiveMinerURLs       ( size_t sampleSize ) const;
    void                                setChainRecorder            ( shared_ptr < AbstractChainRecorder > chainRecorder );
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
