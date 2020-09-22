// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SIMULATION_SIMMININGMESSENGER_H
#define VOLITION_SIMULATION_SIMMININGMESSENGER_H

#include <volition/common.h>
#include <volition/AbstractMiningMessenger.h>
#include <volition/Miner.h>

namespace Volition {
namespace Simulation {

class SimGetBlockTask;

//================================================================//
// SimMiningMessenger
//================================================================//
class SimMiningMessenger :
    public virtual AbstractMiningMessenger {
protected:

    map < string, shared_ptr < Miner >>     mMiners;
    list < shared_ptr < SimGetBlockTask >>  mTasks;

    //----------------------------------------------------------------//
    void        handleTask                  ( SimGetBlockTask& task );

    //----------------------------------------------------------------//
    void        AbstractMiningMessenger_requestBlock        ( AbstractMiningMessengerClient& client, string minerID, string url, size_t height ) override;

public:

    //----------------------------------------------------------------//
                SimMiningMessenger          ();
                ~SimMiningMessenger         ();
    void        setMiners                   ( vector < shared_ptr < Miner >> miners );
    void        updateAndDispatch           ();
};

} // namespace Simulation
} // namespace Volition
#endif
