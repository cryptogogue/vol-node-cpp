// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SIMULATION_SIMULATORACTIVITY_H
#define VOLITION_SIMULATION_SIMULATORACTIVITY_H

#include <volition/simulation/Simulator.h>

namespace Volition {
namespace Simulation {

//================================================================//
// SimulatorActivity
//================================================================//
class SimulatorActivity :
    public Simulator,
    public Poco::Activity < SimulatorActivity > {
private:

    //----------------------------------------------------------------//
    void            runActivity                 ();

public:

    //----------------------------------------------------------------//
                    SimulatorActivity           ();
                    ~SimulatorActivity          ();
    void            shutdown                    ();
};

} // namespace Simulation
} // namespace Volition
#endif
