// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <gtest/gtest.h>
#include <volition/simulation/Simulations.h>
#include <volition/TheContext.h>

using namespace Volition;

//----------------------------------------------------------------//
TEST ( Simulation, small_simulation ) {

    Volition::Simulation::SmallSimulation simulation;
    simulation.run ( 64 );
    ASSERT_TRUE ( simulation.getAnalysis ().getLevelPercent ( 0 ) >= 0.9 );
}

//----------------------------------------------------------------//
TEST ( Simulation, simple_simulation ) {
    
    Volition::Simulation::SimpleSimulation simulation;
    simulation.run ( 256 );
//    float percent = simulation.getAnalysis ().getLevelPercent ( 0 );
    ASSERT_TRUE ( simulation.getAnalysis ().getLevelPercent ( 0 ) >= 0.9 );
}
