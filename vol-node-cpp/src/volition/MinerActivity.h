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

    u32                 mFixedUpdateDelayInMillis;
    u32                 mVariableUpdateDelayInMillis;
    Poco::Event         mShutdownEvent;

    //----------------------------------------------------------------//
    void                runActivity                 ();

    //----------------------------------------------------------------//
    void                Miner_reset                 () override;
    void                Miner_shutdown              ( bool kill ) override;

public:

    static const u32    DEFAULT_FIXED_UPDATE_MILLIS         = 500;
    static const u32    DEFAULT_VARIABLE_UPDATE_MILLIS      = 1000;

    GET_SET ( u32,      FixedUpdateDelayInMillis,       mFixedUpdateDelayInMillis )
    GET_SET ( u32,      VariableUpdateDelayInMillis,    mVariableUpdateDelayInMillis )

    //----------------------------------------------------------------//
    void                waitForShutdown             ();
                        MinerActivity               ();
                        ~MinerActivity              ();
};

} // namespace Volition
#endif
