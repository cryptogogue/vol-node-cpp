// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SIMULATOR_SIMULATION_H
#define VOLITION_SIMULATOR_SIMULATION_H

#include <volition/common.h>
#include <volition/Singleton.h>
#include <volition/simulation/Analysis.h>
#include <volition/simulation/SimMiner.h>

namespace Volition {
namespace Simulation {

class Cohort;
class Player;
class TreeSummary;
class SimMiner;

//================================================================//
// Simulation
//================================================================//
class Simulation {
private:
    
    friend class SimMiner;
    
    CryptoKey                               mGenesisKey;
    
    Analysis                                mAnalysis;
    vector < unique_ptr < SimMiner >>       mMiners;
    float                                   mDropRate;
    int                                     mCyclesPerStep;
    bool                                    mRandomizeScore;
    default_random_engine                   mRand;
    
    //----------------------------------------------------------------//
    bool                drop                    ();
    void                resetMinerQueue         ( vector < int >& minerQueue, bool shuffle );
    void                step                    ( size_t step );

protected:

    //----------------------------------------------------------------//
    virtual bool        Simulation_control      ( size_t step );
    virtual void        Simulation_report       ( size_t step ) const;
    
public:
    
    //----------------------------------------------------------------//
    void                applyCohort             ( Cohort& cohort, string name, int baseMinerID, int size = 1 );
    int                 countMiners             () const;
    const SimMiner&     getMiner                ( int minerID ) const;
    void                initMiners              ( int nMiners );
    void                logMiners               ( string prefix ) const;
    void                logTree                 ( string prefix, bool verbose = false, int maxDepth = 0 ) const;
    size_t              rand                    ();
    void                reset                   ();
    void                run                     ();
    void                run                     ( size_t iterations, bool force = false );
    void                setCyclesPerStep        ( int cycles );
    void                setDropRate             ( float percentage );
    void                setPlayerVerbose        ( int minerID, bool verbose );
    void                setScoreRandomizer      ( bool randomize );
                        Simulation              ();
    virtual             ~Simulation             ();
};

} // namespace Simulator
} // namespace Volition
#endif
