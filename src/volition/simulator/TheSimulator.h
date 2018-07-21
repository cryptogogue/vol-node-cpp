// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SIMULATOR_THESIMULATOR_H
#define VOLITION_SIMULATOR_THESIMULATOR_H

#include <volition/common.h>
#include <volition/Singleton.h>
#include <volition/simulator/SimMiner.h>

namespace Volition {
namespace Simulator {

class Cohort;
class Player;
class TreeSummary;
class SimMiner;

//================================================================//
// TheSimulator
//================================================================//
class TheSimulator :
    public Singleton < TheSimulator > {
private:
    
    friend class SimMiner;
    
    CryptoKey                               mGenesisKey;
    
    vector < unique_ptr < SimMiner >>       mMiners;
    float                                   mDropRate;
    int                                     mCyclesPerStep;
    bool                                    mRandomizeScore;
    default_random_engine                   mRand;
    
    //----------------------------------------------------------------//
    bool                drop                    ();
    void                resetMinerQueue         ( vector < int >& minerQueue, bool shuffle );
    
public:
    
    //----------------------------------------------------------------//
    void                applyCohort             ( Cohort& cohort, string name, int baseMinerID, int size = 1 );
    int                 countMiners             ();
    const SimMiner&     getMiner                ( int minerID );
    void                initMiners              ( int nMiners );
    void                print                   ();
    void                printTree               ( bool verbose = false, int maxDepth = 0 );
    void                process                 ();
    void                reset                   ();
    void                setCyclesPerStep        ( int cycles );
    void                setDropRate             ( float percentage );
    void                setPlayerVerbose        ( int minerID, bool verbose );
    void                setScoreRandomizer      ( bool randomize );
    void                summarize               ( TreeSummary& summary );
};

} // namespace Simulator
} // namespace Volition
#endif
