// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <gtest/gtest.h>
#include <volition/Format.h>

using namespace Volition;

//----------------------------------------------------------------//
//TEST ( Simulation, small_simulation ) {
//
//    Volition::Simulation::SmallSimulation simulation;
//    simulation.run ( 64 );
//    ASSERT_TRUE ( simulation.getAnalysis ().getLevelPercent ( 0 ) >= 0.9 );
//}

//----------------------------------------------------------------//
TEST ( Format, iso8601 ) {
    
    time_t in;
    time ( &in );
    string iso8601 = Format::toISO8601 ( in );
    time_t out = Format::fromISO8601 ( iso8601 );

    ASSERT_TRUE ( in == out );
}
