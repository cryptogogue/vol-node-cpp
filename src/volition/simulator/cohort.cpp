// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/simulator/Cohort.h>
#include <volition/simulator/SimMiner.h>
#include <volition/simulator/TheSimulator.h>

namespace Volition {
namespace Simulator {

//================================================================//
// Cohort
//================================================================//

//----------------------------------------------------------------//
Cohort::Cohort () :
    mBasePlayer ( 0 ),
    mIsPaused ( false ),
    mGroupFlags ( -1 ),
    mRequestFlags ( -1 ),
    mRespondFlags ( -1 ) {
}

//----------------------------------------------------------------//
void Cohort::pause ( bool paused ) {

    this->mIsPaused = paused;
}

//----------------------------------------------------------------//
void Cohort::randomizeFrequencies ( int max ) {

    list < SimMiner* >::iterator minerIt = this->mMiners.begin ();
    for ( ; minerIt != this->mMiners.end (); ++minerIt ) {
        ( *minerIt )->mFrequency = ( rand () % max ) + 1;
    }
}

//----------------------------------------------------------------//
void Cohort::setFlags ( int group, int request, int respond ) {

    this->mGroupFlags       = group;
    this->mRequestFlags     = request;
    this->mRespondFlags     = respond;
}

//----------------------------------------------------------------//
void Cohort::setFrequency ( int frequency ) {

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

