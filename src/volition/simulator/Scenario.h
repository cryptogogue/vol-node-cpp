// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SIMULATOR_SCENARIO_H
#define VOLITION_SIMULATOR_SCENARIO_H

#include <volition/TheContext.h>
#include <volition/simulator/Analysis.h>
#include <volition/simulator/Cohort.h>
#include <volition/simulator/TheSimulator.h>

namespace Volition {
namespace Simulator {

//================================================================//
// Scenario
//================================================================//
class Scenario {

    Analysis    mAnalysis;

public:

    //----------------------------------------------------------------//
    void run () {
    
        for ( int i = 0; this->Scenario_control ( i ); ++i ) {
    
            TheSimulator::get ().process ();
            this->mAnalysis.update ();
            this->Scenario_report ( i );
        }
    }
    
    //----------------------------------------------------------------//
    virtual void Scenario_report ( int i ) {
        printf ( "ROUND: %d - ", i );
        this->mAnalysis.print ( false, 1 );
        TheSimulator::get ().print ();
        printf ( "\n" );
    }
    
    //----------------------------------------------------------------//
    virtual bool    Scenario_control                        ( int step ) = 0;
};

//================================================================//
// FastGangScenario
//================================================================//
class FastGangScenario :
    public Scenario {

    Cohort mRogue;
    Cohort mNormal;

public:

    //----------------------------------------------------------------//
    FastGangScenario () {
    
        TheSimulator::get ().reset ();
        TheSimulator::get ().setScoreRandomizer ( true );
        TheSimulator::get ().initMiners ( 16 );
        
        TheSimulator::get ().applyCohort ( this->mRogue, "RGUE", 0, 4 );
        this->mRogue.setFlags ( 1, 1, 3 );
        this->mRogue.setFrequency ( 16 );
        
        TheSimulator::get ().applyCohort ( this->mNormal, "NORM", 4, 12 );
        this->mNormal.setFlags ( 2, 3, 3 );
        
        //TheSimulator::get ().setPlayerVerbose ( 4, true );
    }

    //================================================================//
    // Scenario
    //================================================================//

    //----------------------------------------------------------------//
    bool Scenario_control ( int step ) {
        
        //return step < 1024;
        return true;
    }
};

//================================================================//
// LateJoinPairScenario
//================================================================//
class LateJoinPairScenario :
    public Scenario {

    Cohort mNormal;
    Cohort mLate;

public:

    //----------------------------------------------------------------//
    LateJoinPairScenario () {
    
        TheSimulator::get ().reset ();
        TheSimulator::get ().initMiners ( 2 );
        TheSimulator::get ().applyCohort ( this->mNormal, "NORM", 0, 1 );
        TheSimulator::get ().applyCohort ( this->mLate, "LATE", 1, 1 );
        
        //this->mNormal.setVerbose ( true );
        //this->mLate.setVerbose ( true );
    
        this->mLate.pause ( true );
    }

    //================================================================//
    // Scenario
    //================================================================//

    //----------------------------------------------------------------//
    bool Scenario_control ( int step ) {
        
        if ( step == 16 ) {
            this->mLate.pause ( false );
        }
        return step < 32;
    }
};

//================================================================//
// LateJoinQuadScenario
//================================================================//
class LateJoinQuadScenario :
    public Scenario {

    Cohort mNormal;
    Cohort mLate;

public:

    //----------------------------------------------------------------//
    LateJoinQuadScenario () {
    
        TheSimulator::get ().reset ();
        //TheSimulator::get ().setThreshold ( 0.3 );
        TheSimulator::get ().initMiners ( 4 );
        TheSimulator::get ().applyCohort ( this->mNormal, "NORM", 0, 3 );
        TheSimulator::get ().applyCohort ( this->mLate, "LATE", 3, 1 );
    
        this->mLate.pause ( true );
    }

    //================================================================//
    // Scenario
    //================================================================//

    //----------------------------------------------------------------//
    bool Scenario_control ( int step ) {
        
        if ( step == 64 ) {
            this->mLate.pause ( false );
        }
        return step < 128;
    }
};

//================================================================//
// RandFreqScenario
//================================================================//
class RandFreqScenario :
    public Scenario {

    Cohort mRandFreq;

public:

    //----------------------------------------------------------------//
    RandFreqScenario () {
    
        TheSimulator::get ().reset ();
        TheSimulator::get ().initMiners ( 32 );
        
        TheSimulator::get ().applyCohort ( this->mRandFreq, "RANDFREQ", 0, 32 );
        this->mRandFreq.randomizeFrequencies ( 10 );
    }

    //================================================================//
    // Scenario
    //================================================================//

    //----------------------------------------------------------------//
    bool Scenario_control ( int step ) {
    
        //return step < 64;
        return true;
    }
};

//================================================================//
// RogueScenario
//================================================================//
class RogueScenario :
    public Scenario {

    Cohort mRogue;
    Cohort mNormal;

public:

    //----------------------------------------------------------------//
    RogueScenario () {
    
        TheSimulator::get ().reset ();
        TheSimulator::get ().initMiners ( 16 );
        TheSimulator::get ().setDropRate ( 0.8 );
        
        TheSimulator::get ().applyCohort ( this->mRogue, "RGUE", 0, 4 );
        this->mRogue.setFlags ( 1, 3, 3 );
        this->mRogue.setFrequency ( 16 );
        
        TheSimulator::get ().applyCohort ( this->mNormal, "NORM", 4, 12 );
        this->mNormal.setFlags ( 2, 3, 3 );
    }

    //================================================================//
    // Scenario
    //================================================================//

    //----------------------------------------------------------------//
    bool Scenario_control ( int step ) {
    
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
// SimpleScenario
//================================================================//
class SimpleScenario :
    public Scenario {

    Cohort mNormal;

public:

    //----------------------------------------------------------------//
    SimpleScenario () {
    
        TheContext::get ().setScoringMode ( TheContext::ScoringMode::INTEGER );
    
        TheSimulator::get ().reset ();
        TheSimulator::get ().initMiners ( 16 );
        TheSimulator::get ().applyCohort ( this->mNormal, "NORM", 0, 16 );
        
        //this->mNormal.setVerbose ( true );
    }

    //================================================================//
    // Scenario
    //================================================================//

    //----------------------------------------------------------------//
    bool Scenario_control ( int step ) {
        
        //return step < 32;
        return true;
    }
};

//================================================================//
// SleepyScenario
//================================================================//
class SleepyScenario :
    public Scenario {

    Cohort mSleepy;
    Cohort mNormal;

public:

    //----------------------------------------------------------------//
    SleepyScenario () {
    
        TheSimulator::get ().reset ();
        TheSimulator::get ().initMiners ( 16 );
        
        TheSimulator::get ().applyCohort ( this->mSleepy, "SLPY", 0, 12 );
        TheSimulator::get ().applyCohort ( this->mNormal, "NORM", 12, 4 );
    }

    //================================================================//
    // Scenario
    //================================================================//

    //----------------------------------------------------------------//
    bool Scenario_control ( int step ) {
    
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
// SmallScenario
//================================================================//
class SmallScenario :
    public Scenario {

    Cohort mNormal;

public:

    //----------------------------------------------------------------//
    SmallScenario () {
    
        TheContext::get ().setScoringMode ( TheContext::ScoringMode::INTEGER );
    
        TheSimulator::get ().reset ();
        TheSimulator::get ().initMiners ( 4 );
        TheSimulator::get ().applyCohort ( this->mNormal, "NORM", 0, 16 );
        
        this->mNormal.setVerbose ( true );
    }

    //================================================================//
    // Scenario
    //================================================================//

    //----------------------------------------------------------------//
    bool Scenario_control ( int step ) {
        
        //return step < 32;
        return true;
    }
};

//================================================================//
// TenKScenario
//================================================================//
class TenKScenario :
    public Scenario {

    Cohort mNormal;

public:

    //----------------------------------------------------------------//
    TenKScenario () {
    
        TheSimulator::get ().reset ();
        TheSimulator::get ().initMiners ( 10000 );
        TheSimulator::get ().applyCohort ( this->mNormal, "NORM", 0, 10000 );
    }

    //================================================================//
    // Scenario
    //================================================================//

    //----------------------------------------------------------------//
    bool Scenario_control ( int step ) {
        
        return true;
    }
    
    //----------------------------------------------------------------//
    void Scenario_report ( int i ) {
        printf ( "ROUND: %d - ", i );
        TheSimulator::get ().printTree ( false, 1 );
    }
};

} // namespace Simulator
} // namespace Volition
#endif
