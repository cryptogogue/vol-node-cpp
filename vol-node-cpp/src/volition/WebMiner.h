// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINER_H
#define VOLITION_WEBMINER_H

#include <volition/common.h>
#include <volition/Block.h>
#include <volition/Chain.h>
#include <volition/Ledger.h>
#include <volition/Miner.h>
#include <volition/Transaction.h>

namespace Volition {

class SyncChainTask;

//================================================================//
// RemoteMiner
//================================================================//
class RemoteMiner {
private:

    friend class WebMiner;

    string                  mURL;
    size_t                  mCurrentBlock;

    bool                    mWaitingForTask;

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

    friend class WebMiner;
    friend class SyncChainTask;

    string      mMinerID;
    Block       mBlock;
    bool        mHasBlock;
};

//================================================================//
// WebMiner
//================================================================//
class WebMiner :
    public Miner,
    public Poco::Activity < WebMiner > {
private:

    SerializableTime                    mStartTime;
    Poco::Mutex                         mMutex;

    Poco::ThreadPool                    mTaskManagerThreadPool;
    Poco::TaskManager                   mTaskManager;
    set < string >                      mMinerSet;
    map < string, RemoteMiner >         mRemoteMiners;

    bool                                        mSolo;
    u32                                         mUpdateIntervalInSeconds;
    list < unique_ptr < BlockQueueEntry >>      mBlockQueue;
    size_t                                      mHeight;
    
    Poco::Event                         mShutdownEvent;

    //----------------------------------------------------------------//
    void                onSyncChainNotification     ( Poco::TaskFinishedNotification* pNf );
    void                processQueue                ();
    void                runActivity                 ();
    void                runMulti                    ();
    void                runSolo                     ();
    void                startTasks                  ();
    void                updateMiners                ();

    //----------------------------------------------------------------//
    void                Miner_reset                 () override;
    void                Miner_shutdown              ( bool kill ) override;

public:

    static const u32                    DEFAULT_UPDATE_INTERVAL = 60;

    //----------------------------------------------------------------//
    Poco::Mutex&        getMutex                    ();
    SerializableTime    getStartTime                ();
    void                setSolo                     ( bool solo );
    void                setUpdateInterval           ( u32 updateIntervalInSeconds );
    void                waitForShutdown             ();
                        WebMiner                    ();
                        ~WebMiner                   ();
};

} // namespace Volition
#endif
