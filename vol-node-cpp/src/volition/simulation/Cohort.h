// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SIMULATOR_COHORT_H
#define VOLITION_SIMULATOR_COHORT_H

#include <volition/common.h>

namespace Volition {
namespace Simulation {

class SimMiner;

//================================================================//
// Cohort
//================================================================//
class Cohort {
private:

    friend class SimMiner;
    friend class Simulation;

    string  mName;

    size_t  mBasePlayer;
    
    bool    mIsPaused;
    u64     mGroupFlags;
    u64     mRequestFlags;
    u64     mRespondFlags;

    list < SimMiner* > mMiners;

public:

    //----------------------------------------------------------------//
                    Cohort                  ();
    void            pause                   ( bool paused );
    void            randomizeFrequencies    ( size_t max );
    void            setFlags                ( u64 group, u64 request, u64 respond );
    void            setFrequency            ( size_t frequency );
    void            setName                 ( string name );
    void            setVerbose              ( bool verbose );
};

} // namespace Simulator
} // namespace Volition
#endif
