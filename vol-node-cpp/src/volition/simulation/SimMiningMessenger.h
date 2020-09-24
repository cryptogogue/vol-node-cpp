// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SIMULATION_SIMMININGMESSENGER_H
#define VOLITION_SIMULATION_SIMMININGMESSENGER_H

#include <volition/common.h>
#include <volition/AbstractMiningMessenger.h>
#include <volition/Miner.h>

namespace Volition {
namespace Simulation {

//================================================================//
// SimMiningMessenger
//================================================================//
class SimMiningMessenger :
    public virtual AbstractMiningMessenger {
protected:

    map < string, shared_ptr < Miner >>                 mMiners;
    list < shared_ptr < MiningMessengerRequest >>       mTasks;

    //----------------------------------------------------------------//
    void        handleTask                  ( const MiningMessengerRequest& task );

    //----------------------------------------------------------------//
    void        AbstractMiningMessenger_request         ( const MiningMessengerRequest& request ) override;

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
