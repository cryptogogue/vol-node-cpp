// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SIMULATOR_SIMMINER_H
#define VOLITION_SIMULATOR_SIMMINER_H

#include <common.h>
#include <Miner.h>

namespace Volition {
namespace Simulator {

class Cohort;

//================================================================//
// SimMiner
//================================================================//
class SimMiner :
    public Miner {
private:

    friend class Cohort;
    friend class TheSimulator;

    Cohort*                 mCohort;
    int                     mFrequency;
    bool                    mVerbose;

    size_t                  mMinerCursor; // where we are in the check
    vector < int >          mMinerQueue; // miner ID's to check

    //----------------------------------------------------------------//
    void                pushGenesis                 ( Block& block );
    const SimMiner*     nextMiner                   ();
    void                resetMinerQueue             ();

public:

    //----------------------------------------------------------------//
    void                print                       () const;
    void                step                        ();
                        SimMiner                    ();
                        ~SimMiner                   ();
};

} // namespace Simulator
} // namespace Volition
#endif
