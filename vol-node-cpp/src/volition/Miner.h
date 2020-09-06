// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_MINER_H
#define VOLITION_MINER_H

#include <volition/common.h>
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
class Block;
class BlockTreeNode;

//================================================================//
// RemoteMiner
//================================================================//
class RemoteMiner {
private:

    friend class Miner;

    string                  mURL;
    size_t                  mCurrentBlock;

    bool                    mWaitingForTask;

    BlockTreeTag            mTag;

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

    friend class Miner;
    friend class SyncChainTask;

    string                      mMinerID;
    shared_ptr < const Block >  mBlock;
};

//================================================================//
// Miner
//================================================================//
class Miner :
    public AbstractSerializable,
    public TransactionQueue {
protected:

    friend class AbstractChainRecorder;

    static constexpr const char* MASTER_BRANCH      = "master";

    string                                          mMinerID;

    CryptoKey                                       mKeyPair;
    bool                                            mLazy;
    bool                                            mSolo;
    bool                                            mVerbose;
    bool                                            mControlPermitted;

    Block::VerificationPolicy                       mBlockVerificationPolicy;

    shared_ptr < AbstractChainRecorder >            mChainRecorder;
    
    map < string, RemoteMiner >                     mRemoteMiners;
    BlockTree                                       mBlockTree;
    shared_ptr < Chain >                            mChain;
    BlockTreeTag                                    mTag;
    
    Poco::Mutex                                     mMutex;

    Poco::ThreadPool                                mTaskManagerThreadPool;
    Poco::TaskManager                               mTaskManager;
    set < string >                                  mMinerSet;

    list < unique_ptr < BlockQueueEntry >>          mBlockQueue;
    
    //----------------------------------------------------------------//
    static void             checkEnvironment            ();
    void                    discoverMiners              ();
    void                    processQueue                ();
    void                    onSyncChainNotification     ( Poco::TaskFinishedNotification* pNf );
    void                    pushBlock                   ( shared_ptr < const Block > block );
    void                    rebuildChain                ( shared_ptr < const BlockTreeNode > original, shared_ptr < const BlockTreeNode > replace );
    void                    rebuildChainRecurse         ( shared_ptr < const BlockTreeNode > node, shared_ptr < const BlockTreeNode > root );
    void                    saveChain                   ();
    void                    startTasks                  ();

    //----------------------------------------------------------------//
    void                    AbstractSerializable_serializeFrom      ( const AbstractSerializerFrom& serializer ) override;
    void                    AbstractSerializable_serializeTo        ( AbstractSerializerTo& serializer ) const override;
    virtual time_t          Miner_getTime                           () const;
    virtual void            Miner_reset                             ();
    virtual void            Miner_shutdown                          ( bool kill );

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
    void                    affirmKey                   ();
    bool                    checkBestBranch             ( string miners ) const;
    bool                    controlPermitted            () const;
    size_t                  countBranches               () const;
    void                    extend                      ();
    const Chain*            getBestBranch               () const;
    const BlockTree&        getBlockTree                () const;
    const BlockTreeTag&     getBlockTreeTag             () const;
    const CryptoKey&        getKeyPair                  () const;
    size_t                  getLongestBranchSize        () const;
    bool                    getLazy                     () const;
    Ledger&                 getLedger                   ();
    const Ledger&           getLedger                   () const;
    string                  getMinerID                  () const;
    time_t                  getTime                     () const;
    void                    loadGenesis                 ( string path );
    void                    loadKey                     ( string keyfile, string password = "" );
                            Miner                       ();
    virtual                 ~Miner                      ();
    void                    permitControl               ( bool permit );
    shared_ptr < Block >    prepareBlock                ();
    void                    setChainRecorder            ( shared_ptr < AbstractChainRecorder > chainRecorder );
    void                    setGenesis                  ( shared_ptr < const Block > block );
    void                    setLazy                     ( bool lazy );
    void                    setMinerID                  ( string minerID );
    void                    setSolo                     ( bool solo );
    void                    setVerbose                  ( bool verbose );
    void                    reset                       ();
    void                    shutdown                    ( bool kill = false );
    void                    step                        ( bool solo = false );
};

//================================================================//
// ScopedWebMinerLock
//================================================================//
class ScopedWebMinerLock {
private:

    shared_ptr < Miner >                mWebMiner;
    Poco::ScopedLock < Poco::Mutex >    mScopedLock;

public:

    //----------------------------------------------------------------//
    ScopedWebMinerLock ( shared_ptr < Miner > webMiner ) :
        mWebMiner ( webMiner ),
        mScopedLock ( *webMiner ) {
    }
};

} // namespace Volition
#endif
