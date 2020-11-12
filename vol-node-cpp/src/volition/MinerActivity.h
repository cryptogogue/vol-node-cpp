// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_MINERACTIVITY_H
#define VOLITION_MINERACTIVITY_H

#include <volition/common.h>
#include <volition/Block.h>
#include <volition/BlockTree.h>
#include <volition/Ledger.h>
#include <volition/Miner.h>
#include <volition/Transaction.h>

namespace Volition {

//================================================================//
// MinerActivity
//================================================================//
class MinerActivity :
    public Miner,
    public Poco::Activity < MinerActivity > {
private:

    u32                 mUpdateIntervalInSeconds;
    Poco::Event         mShutdownEvent;

    //----------------------------------------------------------------//
    void                runActivity                 ();

    //----------------------------------------------------------------//
    void                Miner_reset                 () override;
    void                Miner_shutdown              ( bool kill ) override;

public:

    static const u32    DEFAULT_UPDATE_INTERVAL = 1;

    //----------------------------------------------------------------//
    void                setUpdateInterval           ( u32 updateIntervalInSeconds );
    void                waitForShutdown             ();
                        MinerActivity               ();
                        ~MinerActivity              ();
};

} // namespace Volition
#endif
