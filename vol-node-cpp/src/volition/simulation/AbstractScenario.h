// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SIMULATION_ABSTRACTSCENARIO_H
#define VOLITION_SIMULATION_ABSTRACTSCENARIO_H

#include <volition/common.h>
#include <volition/Block.h>
#include <volition/BlockTree.h>
#include <volition/Ledger.h>
#include <volition/Miner.h>
#include <volition/simulation/Analysis.h>
#include <volition/simulation/SimMiningMessenger.h>
#include <volition/simulation/Simulator.h>
#include <volition/Transaction.h>

namespace Volition {
namespace Simulation {

//================================================================//
// AbstractScenario
//================================================================//
class AbstractScenario {
protected:

    friend class Simulator;

    //----------------------------------------------------------------//
    virtual void AbstractScenario_control ( Simulator& simulator, SimMiningNetwork& network, size_t step ) {
        UNUSED ( simulator );
        UNUSED ( network );
        UNUSED ( step );
    }
    
    //----------------------------------------------------------------//
    virtual void AbstractScenario_setup ( Simulator& simulator ) {
        UNUSED ( simulator );
    }

public:

    //----------------------------------------------------------------//
    AbstractScenario () {
    }
    
    //----------------------------------------------------------------//
    virtual ~AbstractScenario () {
    }
};

} // namespace Simulation
} // namespace Volition
#endif
