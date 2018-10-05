// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINER_H
#define VOLITION_WEBMINER_H

#include <volition/common.h>
#include <volition/AbstractTransaction.h>
#include <volition/Chain.h>
#include <volition/Miner.h>
#include <volition/State.h>

namespace Volition {

class SyncChainTask;

//================================================================//
// WebMiner
//================================================================//
class WebMiner :
    public Miner,
    public Poco::Activity < WebMiner > {
private:

    Poco::TaskManager                           mTaskManager;
    map < string, string >                      mMinerURLs;

    bool                                        mSolo;

    Poco::Mutex                                 mChainMutex;

    //----------------------------------------------------------------//
    void            onSyncChainNotification     ( Poco::TaskFinishedNotification* pNf );
    void            run                         () override;
    void            updateChains                ();

public:

    //----------------------------------------------------------------//
    const Chain&    lockChain               ();
    void            setSolo                 ( bool solo );
    void            shutdown                ();
    void            unlockChain             ();
                    WebMiner                ();
                    ~WebMiner               ();
};

} // namespace Volition
#endif
