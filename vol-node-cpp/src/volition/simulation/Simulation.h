// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SIMULATOR_SIMULATION_H
#define VOLITION_SIMULATOR_SIMULATION_H

#include <volition/common.h>
#include <volition/Singleton.h>
#include <volition/simulation/Analysis.h>
#include <volition/simulation/SimMiner.h>
#include <volition/TheContext.h>

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
    double                                  mDropRate;
    size_t                                  mCyclesPerStep;
    bool                                    mRandomizeScore;
    default_random_engine                   mRand;
    u64                                     mStepSize;
    
    //----------------------------------------------------------------//
    bool                drop                    ();
    void                resetMinerQueue         ( vector < size_t >& minerQueue, bool shuffle );
    void                step                    ( size_t step );

protected:

    //----------------------------------------------------------------//
    virtual bool        Simulation_control      ( size_t step );
    virtual void        Simulation_report       ( size_t step ) const;
    
public:
    
    //----------------------------------------------------------------//
    void                applyCohort             ( Cohort& cohort, string name, size_t baseMinerID, size_t size = 1 );
    size_t              countMiners             () const;
    const Analysis&     getAnalysis             () const;
    const SimMiner&     getMiner                ( size_t minerID ) const;
    void                initMiners              ( size_t nMiners, TheContext::ScoringMode scoringMode = TheContext::ScoringMode::ALLURE, double window = 600, u64 stepSize = 6 );
    void                logMiners               ( string prefix ) const;
    void                logTree                 ( string prefix, bool verbose = false, size_t maxDepth = 0 ) const;
    size_t              rand                    ();
    void                reset                   ();
    void                run                     ();
    void                run                     ( size_t iterations, bool force = false );
    void                setCyclesPerStep        ( size_t cycles );
    void                setDropRate             ( double percentage );
    void                setPlayerVerbose        ( size_t minerID, bool verbose );
    void                setScoreRandomizer      ( bool randomize );
    void                setStepSize             ( u64 stepSize );
                        Simulation              ();
    virtual             ~Simulation             ();
};

} // namespace Simulator
} // namespace Volition
#endif
