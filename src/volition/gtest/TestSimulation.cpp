// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <gtest/gtest.h>
#include <volition/simulation/Simulations.h>
#include <volition/TheContext.h>

using namespace Volition;

//----------------------------------------------------------------//
TEST ( Simulation, test0 ) {
    
    Volition::Simulation::SmallSimulation simulation;
    simulation.run ( 64 );
    ASSERT_TRUE ( simulation.getAnalysis ().getLevelPercent ( 0 ) >= 0.9 );
}
