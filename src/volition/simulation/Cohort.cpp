// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/simulation/Cohort.h>
#include <volition/simulation/SimMiner.h>
#include <volition/simulation/Simulation.h>

namespace Volition {
namespace Simulation {

//================================================================//
// Cohort
//================================================================//

//----------------------------------------------------------------//
Cohort::Cohort () :
    mBasePlayer ( 0 ),
    mIsPaused ( false ),
    mGroupFlags (( u64 )-1 ),
    mRequestFlags (( u64 )-1 ),
    mRespondFlags (( u64 )-1 ) {
}

//----------------------------------------------------------------//
void Cohort::pause ( bool paused ) {

    this->mIsPaused = paused;
}

//----------------------------------------------------------------//
void Cohort::randomizeFrequencies ( size_t max ) {

    list < SimMiner* >::iterator minerIt = this->mMiners.begin ();
    for ( ; minerIt != this->mMiners.end (); ++minerIt ) {
        SimMiner* miner = *minerIt;
        miner->mFrequency = ( miner->mSimulation.rand () % max ) + 1;
    }
}

//----------------------------------------------------------------//
void Cohort::setFlags ( u64 group, u64 request, u64 respond ) {

    this->mGroupFlags       = group;
    this->mRequestFlags     = request;
    this->mRespondFlags     = respond;
}

//----------------------------------------------------------------//
void Cohort::setFrequency ( size_t frequency ) {

    list < SimMiner* >::iterator minerIt = this->mMiners.begin ();
    for ( ; minerIt != this->mMiners.end (); ++minerIt ) {
        ( *minerIt )->mFrequency = frequency;
    }
}

//----------------------------------------------------------------//
void Cohort::setName ( string name ) {

    this->mName = name;
}

//----------------------------------------------------------------//
void Cohort::setVerbose ( bool verbose ) {

    list < SimMiner* >::iterator minerIt = this->mMiners.begin ();
    for ( ; minerIt != this->mMiners.end (); ++minerIt ) {
        ( *minerIt )->mVerbose = verbose;
    }
}

} // namespace Simulator
} // namespace Volition

