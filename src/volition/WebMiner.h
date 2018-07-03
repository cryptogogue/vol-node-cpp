// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINER_H
#define VOLITION_WEBMINER_H

#include "common.h"
#include "AbstractTransaction.h"
#include "Chain.h"
#include "Miner.h"
#include "State.h"

namespace Volition {

class AbstractHashable;
class Signable;
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

    //----------------------------------------------------------------//
    void            onSyncChainNotification     ( Poco::TaskFinishedNotification* pNf );
    void            run                         () override;

public:

    //----------------------------------------------------------------//
    void            shutdown                ();
                    WebMiner                ();
                    ~WebMiner               ();
};

} // namespace Volition
#endif
