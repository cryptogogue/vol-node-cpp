// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SIMULATOR_SIMMINER_H
#define VOLITION_SIMULATOR_SIMMINER_H

#include <volition/common.h>
#include <volition/Miner.h>

namespace Volition {
namespace Simulation {

class Cohort;
class Simulation;

//================================================================//
// SimMiner
//================================================================//
class SimMiner :
    public Miner {
private:

    friend class Cohort;
    friend class Simulation;

    Cohort*                 mCohort;
    Simulation&             mSimulation;
    int                     mFrequency;
    bool                    mVerbose;

    size_t                  mMinerCursor; // where we are in the check
    vector < int >          mMinerQueue; // miner ID's to check

    //----------------------------------------------------------------//
    void                pushGenesisTransaction      ( Block& block ) const;
    const SimMiner*     nextMiner                   ();
    void                resetMinerQueue             ();

public:

    //----------------------------------------------------------------//
    void                log                         ( string prefix ) const;
    void                step                        ();
                        SimMiner                    ( Simulation& simulation );
                        ~SimMiner                   ();
};

} // namespace Simulator
} // namespace Volition
#endif
