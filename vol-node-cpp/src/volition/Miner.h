// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_MINER_H
#define VOLITION_MINER_H

#include <volition/common.h>
#include <volition/AbstractMiningMessengerClient.h>
#include <volition/Accessors.h>
#include <volition/BlockTree.h>
#include <volition/CryptoKey.h>
#include <volition/Ledger.h>
#include <volition/TransactionQueue.h>
#include <volition/serialization/AbstractSerializable.h>
#include <volition/Singleton.h>

namespace Volition {

class AbstractChainRecorder;
class AbstractHashable;

//================================================================//
// MinerSearchEntry
//================================================================//
class MinerSearchEntry {
protected:

    friend class Miner;

    BlockTreeNode::ConstPtr     mSearchTarget;
    size_t                      mSearchCount;
    size_t                      mSearchLimit;
};

//================================================================//
// RemoteMiner
//================================================================//
class RemoteMiner {
public:

    enum MinerState {
        STATE_NEW,
        STATE_TIMEOUT,
        STATE_ONLINE,
        STATE_ERROR,
    };

    string                      mMinerID;
    string                      mURL;
    BlockTreeNode::ConstPtr     mTag;
    MinerState                  mState;
    string                      mMessage;

    size_t                                              mHeight;
    bool                                                mForward;
    map < size_t, shared_ptr < const BlockHeader >>     mHeaderQueue;

    //----------------------------------------------------------------//
                    RemoteMiner             ();
                    ~RemoteMiner            ();
    void            setError                ( string message = "" );
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

    enum : int {
        MINER_VERBOSE               = 0x02,
        MINER_MUTE                  = 0x08,
    };

    static const int DEFAULT_FLAGS = 0;

protected:

    friend class AbstractChainRecorder;

    static constexpr const char* MASTER_BRANCH      = "master";

    string                                          mMinerID;
    string                                          mURL;
    SerializableTime                                mStartTime;

    CryptoKeyPair                                   mKeyPair;
    string                                          mMotto;
    Signature                                       mVisage;

    int                                             mFlags;
    BlockTreeNode::RewriteMode                      mRewriteMode;
    time_t                                          mRewriteWindowInSeconds;
    Block::VerificationPolicy                       mBlockVerificationPolicy;

    CryptoPublicKey                                 mControlKey;
    Control                                         mControlLevel;

    MinerConfig                                     mConfig;

    shared_ptr < AbstractChainRecorder >            mChainRecorder;
    
    set < string >                                  mNewMinerURLs;
    set < string >                                  mActiveMinerURLs;
    map < string, shared_ptr < RemoteMiner >>       mRemoteMinersByID;
    map < string, shared_ptr < RemoteMiner >>       mRemoteMinersByURL;
    set < shared_ptr < RemoteMiner >>               mOnlineMiners;
    map < string, MinerSearchEntry >                mBlockSearches;
    set < string >                                  mHeaderSearches;
    
    BlockTree                                       mBlockTree;
    
    shared_ptr < Ledger >                           mChain;         // may run behind block tree tag
    BlockTreeNode::ConstPtr                         mChainTag;      // node corresponding to top of chain
    BlockTreeNode::ConstPtr                         mBestBranch;    // "leaf" of the current chain
    list < BlockTreeNode::ConstPtr >                mNodeQueue;
    
    Poco::Mutex                                     mMutex;

    shared_ptr < AbstractMiningMessenger >          mMessenger;
    list < MiningMessengerResponse >                mResponseQueue;
    
    //----------------------------------------------------------------//
    void                                affirmBranchSearch          ( BlockTreeNode::ConstPtr node );
    void                                affirmMessenger             ();
    void                                affirmNodeSearch            ( BlockTreeNode::ConstPtr node );
    bool                                canExtend                   () const;
    void                                composeChain                ();
    void                                discoverMiners              ();
    void                                processResponses            ();
    void                                pushBlock                   ( shared_ptr < const Block > block );
    void                                requestHeaders              ();
    void                                saveChain                   ();
    void                                saveConfig                  ();
    void                                selectBestBranch            ( time_t now );
    BlockTreeNode::ConstPtr             truncate                    ( BlockTreeNode::ConstPtr tail, time_t now ) const;
    void                                updateChainRecurse          ( BlockTreeNode::ConstPtr branch );
    void                                updateSearches              ( time_t now );    

    //----------------------------------------------------------------//
    void                                AbstractMiningMessengerClient_receiveResponse   ( const MiningMessengerResponse& response ) override;
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
    GET ( const Ledger&,                            Ledger,                     *mChain )
    GET ( u64,                                      MinimumGratuity,            mConfig.mMinimumGratuity )
    GET ( string,                                   Reward,                     mConfig.mReward )
    GET ( time_t,                                   RewriteWindowInSeconds,     mRewriteWindowInSeconds )
    GET ( SerializableTime,                         StartTime,                  mStartTime )
    GET ( const Signature&,                         Visage,                     mVisage )
    
    SET ( shared_ptr < AbstractMiningMessenger >,   Messenger,                  mMessenger )
    
    GET_SET ( const CryptoPublicKey&,               ControlKey,                 mControlKey )
    GET_SET ( Control,                              ControlLevel,               mControlLevel )
    GET_SET ( const CryptoKeyPair&,                 KeyPair,                    mKeyPair )
    GET_SET ( string,                               MinerID,                    mMinerID )
    GET_SET ( string,                               Motto,                      mMotto )
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
    bool                                isLazy                      () const;
    void                                loadGenesis                 ( string path );
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
    void                                setRewriteWindow            ( time_t window );
    void                                setVerbose                  ( bool verbose );
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
