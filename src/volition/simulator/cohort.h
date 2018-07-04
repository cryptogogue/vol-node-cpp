// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SIMULATOR_COHORT_H
#define VOLITION_SIMULATOR_COHORT_H

#include <volition/common.h>

namespace Volition {
namespace Simulator {

class SimMiner;

//================================================================//
// Cohort
//================================================================//
class Cohort {
private:

    friend class TheSimulator;
    friend class SimMiner;

    string  mName;

    int     mBasePlayer;
    bool    mIsPaused;
    
    int     mGroupFlags;
    int     mRequestFlags;
    int     mRespondFlags;

    list < SimMiner* > mMiners;

public:

    //----------------------------------------------------------------//
                    Cohort                  ();
    void            pause                   ( bool paused );
    void            randomizeFrequencies    ( int max );
    void            setFlags                ( int group, int request, int respond );
    void            setFrequency            ( int frequency );
    void            setName                 ( string name );
    void            setVerbose              ( bool verbose );
};

} // namespace Simulator
} // namespace Volition
#endif
