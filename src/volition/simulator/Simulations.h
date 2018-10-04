// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SIMULATOR_SCENARIO_H
#define VOLITION_SIMULATOR_SCENARIO_H

#include <volition/TheContext.h>
#include <volition/simulator/Analysis.h>
#include <volition/simulator/Cohort.h>
#include <volition/simulator/Simulation.h>

namespace Volition {
namespace Simulator {

//================================================================//
// FastGangSimulation
//================================================================//
class FastGangSimulation :
    public Simulation {

    Cohort mRogue;
    Cohort mNormal;

public:

    //----------------------------------------------------------------//
    FastGangSimulation () {
    
        this->setScoreRandomizer ( true );
        this->initMiners ( 16 );
        
        this->applyCohort ( this->mRogue, "RGUE", 0, 4 );
        this->mRogue.setFlags ( 1, 1, 3 );
        this->mRogue.setFrequency ( 16 );
        
        this->applyCohort ( this->mNormal, "NORM", 4, 12 );
        this->mNormal.setFlags ( 2, 3, 3 );
        
        //this->setPlayerVerbose ( 4, true );
    }

    //================================================================//
    // Simulation
    //================================================================//

    //----------------------------------------------------------------//
};

//================================================================//
// LateJoinPairSimulation
//================================================================//
class LateJoinPairSimulation :
    public Simulation {

    Cohort mNormal;
    Cohort mLate;

public:

    //----------------------------------------------------------------//
    LateJoinPairSimulation () {
    
        this->initMiners ( 2 );
        this->applyCohort ( this->mNormal, "NORM", 0, 1 );
        this->applyCohort ( this->mLate, "LATE", 1, 1 );
        
        //this->mNormal.setVerbose ( true );
        //this->mLate.setVerbose ( true );
    
        this->mLate.pause ( true );
    }

    //================================================================//
    // Simulation
    //================================================================//

    //----------------------------------------------------------------//
    bool Simulation_control ( size_t step ) {
        
        if ( step == 16 ) {
            this->mLate.pause ( false );
        }
        return step < 32;
    }
};

//================================================================//
// LateJoinQuadSimulation
//================================================================//
class LateJoinQuadSimulation :
    public Simulation {

    Cohort mNormal;
    Cohort mLate;

public:

    //----------------------------------------------------------------//
    LateJoinQuadSimulation () {
    
        //this->setThreshold ( 0.3 );
        this->initMiners ( 4 );
        this->applyCohort ( this->mNormal, "NORM", 0, 3 );
        this->applyCohort ( this->mLate, "LATE", 3, 1 );
    
        this->mLate.pause ( true );
    }

    //================================================================//
    // Simulation
    //================================================================//

    //----------------------------------------------------------------//
    bool Simulation_control ( size_t step ) {
        
        if ( step == 64 ) {
            this->mLate.pause ( false );
        }
        return step < 128;
    }
};

//================================================================//
// RandFreqSimulation
//================================================================//
class RandFreqSimulation :
    public Simulation {

    Cohort mRandFreq;

public:

    //----------------------------------------------------------------//
    RandFreqSimulation () {
    
        this->initMiners ( 32 );
        
        this->applyCohort ( this->mRandFreq, "RANDFREQ", 0, 32 );
        this->mRandFreq.randomizeFrequencies ( 10 );
    }

    //================================================================//
    // Simulation
    //================================================================//

    //----------------------------------------------------------------//
};

//================================================================//
// RogueSimulation
//================================================================//
class RogueSimulation :
    public Simulation {

    Cohort mRogue;
    Cohort mNormal;

public:

    //----------------------------------------------------------------//
    RogueSimulation () {
    
        this->initMiners ( 16 );
        this->setDropRate ( 0.8 );
        
        this->applyCohort ( this->mRogue, "RGUE", 0, 4 );
        this->mRogue.setFlags ( 1, 3, 3 );
        this->mRogue.setFrequency ( 16 );
        
        this->applyCohort ( this->mNormal, "NORM", 4, 12 );
        this->mNormal.setFlags ( 2, 3, 3 );
    }

    //================================================================//
    // Simulation
    //================================================================//

    //----------------------------------------------------------------//
    bool Simulation_control ( size_t step ) {
    
        switch ( step ) {
            case 0:
                this->mRogue.setFlags ( 1, 1, 1 );
                break;
            
            case 32:
                this->mRogue.setFlags ( 1, 3, 3 );
                break;
        }
        return step < 64;
    }
};

//================================================================//
// SimpleSimulation
//================================================================//
class SimpleSimulation :
    public Simulation {

    Cohort mNormal;

public:

    //----------------------------------------------------------------//
    SimpleSimulation () {
    
        TheContext::get ().setScoringMode ( TheContext::ScoringMode::INTEGER );
    
        this->initMiners ( 16 );
        this->applyCohort ( this->mNormal, "NORM", 0, 16 );
        
        //this->mNormal.setVerbose ( true );
    }

    //================================================================//
    // Simulation
    //================================================================//

    //----------------------------------------------------------------//
};

//================================================================//
// SleepySimulation
//================================================================//
class SleepySimulation :
    public Simulation {

    Cohort mSleepy;
    Cohort mNormal;

public:

    //----------------------------------------------------------------//
    SleepySimulation () {
    
        this->initMiners ( 16 );
        
        this->applyCohort ( this->mSleepy, "SLPY", 0, 12 );
        this->applyCohort ( this->mNormal, "NORM", 12, 4 );
    }

    //================================================================//
    // Simulation
    //================================================================//

    //----------------------------------------------------------------//
    bool Simulation_control ( size_t step ) {
    
        switch ( step ) {
            case 0:
                this->mSleepy.pause ( true );
                break;
            
            case 16:
                this->mSleepy.pause ( false );
                break;
        }
        return step < 32;
    }
};

//================================================================//
// SmallSimulation
//================================================================//
class SmallSimulation :
    public Simulation {

    Cohort mNormal;

public:

    //----------------------------------------------------------------//
    SmallSimulation () {
    
        TheContext::get ().setScoringMode ( TheContext::ScoringMode::INTEGER );
    
        this->initMiners ( 4 );
        this->applyCohort ( this->mNormal, "NORM", 0, 4 );
        
        this->mNormal.setVerbose ( true );
    }

    //================================================================//
    // Simulation
    //================================================================//

    //----------------------------------------------------------------//
};

//================================================================//
// TenKSimulation
//================================================================//
class TenKSimulation :
    public Simulation {

    Cohort mNormal;

public:

    //----------------------------------------------------------------//
    TenKSimulation () {
    
        this->initMiners ( 10000 );
        this->applyCohort ( this->mNormal, "NORM", 0, 10000 );
    }

    //================================================================//
    // Simulation
    //================================================================//

    //----------------------------------------------------------------//
    bool Simulation_control ( size_t step ) {
        
        return true;
    }
    
    //----------------------------------------------------------------//
    void Simulation_report ( size_t step ) const {
        LOG_F ( INFO, "SIM: ROUND: %d - ", ( int )step );
        this->logTree ( "SIM: ", false, 1 );
    }
};

} // namespace Simulator
} // namespace Volition
#endif
