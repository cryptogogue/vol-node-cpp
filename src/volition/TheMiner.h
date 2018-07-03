// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_THEMINER_H
#define VOLITION_THEMINER_H

#include "common.h"
#include "AbstractTransaction.h"
#include "Chain.h"
#include "Miner.h"
#include "Singleton.h"
#include "State.h"

namespace Volition {

class AbstractHashable;
class Signable;
class SyncChainTask;

//================================================================//
// TheMiner
//================================================================//
class TheMiner :
    public Miner,
    public Singleton < TheMiner >,
    public Poco::Activity < TheMiner > {
private:

    Poco::TaskManager                           mTaskManager;
    map < string, string >                      mMinerURLs;

    //----------------------------------------------------------------//
    void            onSyncChainNotification     ( Poco::TaskFinishedNotification* pNf );
    void            run                         () override;

public:

    //----------------------------------------------------------------//
    void            shutdown                ();
                    TheMiner                ();
                    ~TheMiner               ();
};

} // namespace Volition
#endif
