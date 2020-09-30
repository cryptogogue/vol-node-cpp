// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_MINERBASE_H
#define VOLITION_MINERBASE_H

#include <volition/common.h>
#include <volition/AbstractMiningMessenger.h>
#include <volition/BlockTree.h>
#include <volition/CryptoKey.h>
#include <volition/Chain.h>
#include <volition/ChainMetadata.h>
#include <volition/Ledger.h>
#include <volition/TransactionQueue.h>
#include <volition/serialization/AbstractSerializable.h>
#include <volition/Singleton.h>
#include <volition/Transaction.h>

namespace Volition {

class AbstractChainRecorder;
class AbstractHashable;

//================================================================//
// RemoteMiner
//================================================================//
class RemoteMiner {
private:

    friend class MinerBase;
    friend class Miner;

    string                      mURL;
    size_t                      mCurrentBlock;
    
    BlockTreeNode::ConstPtr     mTag;

public:

    //----------------------------------------------------------------//
                    RemoteMiner             ();
                    ~RemoteMiner            ();
};

//================================================================//
// BlockQueueEntry
//================================================================//
class BlockQueueEntry {
private:

    friend class MinerBase;
    friend class Miner;

    MiningMessengerRequest              mRequest;
    shared_ptr < const BlockHeader >    mBlockHeader;
    shared_ptr < const Block >          mBlock;
};

//================================================================//
// MinerBase
//================================================================//
class MinerBase :
    public AbstractSerializable,
    public AbstractMiningMessengerClient,
    public TransactionQueue {
protected:

    friend class AbstractChainRecorder;

    static constexpr const char* MASTER_BRANCH      = "master";

    string                                          mMinerID;
    SerializableTime                                mStartTime;

    CryptoKey                                       mKeyPair;
    string                                          mMotto;
    Signature                                       mVisage;
    
    bool                                            mLazy;
    bool                                            mSolo;
    bool                                            mVerbose;
    bool                                            mControlPermitted;

    Block::VerificationPolicy                       mBlockVerificationPolicy;

    shared_ptr < AbstractChainRecorder >            mChainRecorder;
    
    map < string, RemoteMiner >                     mRemoteMiners;
    BlockTree                                       mBlockTree;
    
    shared_ptr < Chain >                            mChain;         // may run behind block tree tag
    BlockTreeNode::ConstPtr                         mChainTag;      // node corresponding to top of chain
    BlockTreeNode::ConstPtr                         mBestBranch;    // "leaf" of the current chain
    list < BlockTreeNode::ConstPtr >                mNodeQueue;
    
    Poco::Mutex                                     mMutex;

    shared_ptr < AbstractMiningMessenger >          mMessenger;
    set < string >                                  mMinerSet;

    list < unique_ptr < BlockQueueEntry >>          mBlockQueue;
    
    //----------------------------------------------------------------//
    void                        affirmMessenger             ();
    void                        discoverMiners              ();
    void                        pushBlock                   ( shared_ptr < const Block > block );
    void                        saveChain                   ();

    //----------------------------------------------------------------//
    void                        AbstractMiningMessengerClient_receive           ( const MiningMessengerRequest& request, shared_ptr < const BlockHeader > header, shared_ptr < const Block > block ) override;
    void                        AbstractSerializable_serializeFrom              ( const AbstractSerializerFrom& serializer ) override;
    void                        AbstractSerializable_serializeTo                ( AbstractSerializerTo& serializer ) const override;
    virtual time_t              Miner_getTime                                   () const;
    virtual void                Miner_reset                                     ();
    virtual void                Miner_shutdown                                  ( bool kill );

public:

    enum class SubmissionResponse {
        ACCEPTED = 0,
        RESUBMIT_EARLIER,
    };
    
    //----------------------------------------------------------------//
    operator Poco::Mutex& () {
    
        return this->mMutex;
    }

    //----------------------------------------------------------------//
    void                                    affirmKey                   ( uint keyLength = CryptoKey::RSA_1024, unsigned long exp = CryptoKey::RSA_EXP_65537 );
    void                                    affirmVisage                ();
    bool                                    checkBestBranch             ( string miners ) const;
    bool                                    controlPermitted            () const;
    size_t                                  countBranches               () const;
    void                                    extend                      ();
    BlockTreeNode::ConstPtr                 getBestBranch               () const;
    const BlockTree&                        getBlockTree                () const;
    const Chain*                            getChain                    () const;
    const CryptoKey&                        getKeyPair                  () const;
    size_t                                  getLongestBranchSize        () const;
    bool                                    getLazy                     () const;
    Ledger&                                 getLedger                   ();
    const Ledger&                           getLedger                   () const;
    string                                  getMinerID                  () const;
    string                                  getMotto                    () const;
    SerializableTime                        getStartTime                () const;
    time_t                                  getTime                     () const;
    const Signature&                        getVisage                   () const;
    void                                    loadGenesis                 ( string path );
    void                                    loadKey                     ( string keyfile, string password = "" );
                                            MinerBase                   ();
    virtual                                 ~MinerBase                  ();
    void                                    permitControl               ( bool permit );
    shared_ptr < Block >                    prepareBlock                ();
    void                                    reset                       ();
    void                                    setChainRecorder            ( shared_ptr < AbstractChainRecorder > chainRecorder );
    void                                    setGenesis                  ( shared_ptr < const Block > block );
    void                                    setLazy                     ( bool lazy );
    void                                    setMessenger                ( shared_ptr < AbstractMiningMessenger > messenger );
    void                                    setMinerID                  ( string minerID );
    void                                    setMotto                    ( string motto );
    void                                    setSolo                     ( bool solo );
    void                                    setVerbose                  ( bool verbose );
    void                                    shutdown                    ( bool kill = false );
};

//================================================================//
// ScopedMinerLock
//================================================================//
class ScopedMinerLock {
private:

    shared_ptr < MinerBase >            mMiner;
    Poco::ScopedLock < Poco::Mutex >    mScopedLock;

public:

    //----------------------------------------------------------------//
    ScopedMinerLock ( shared_ptr < MinerBase > webMiner ) :
        mMiner ( webMiner ),
        mScopedLock ( *webMiner ) {
    }
};

} // namespace Volition
#endif
