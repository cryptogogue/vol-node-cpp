// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef SCENARIO_H
#define SCENARIO_H


#include <simulator/analysis.h>
#include <simulator/context.h>
#include <simulator/cohort.h>

//================================================================//
// Scenario
//================================================================//
class Scenario {

    Analysis    mAnalysis;

public:

    //----------------------------------------------------------------//
    void Run () {
    
        for ( int i = 0; this->Scenario_Control ( i ); ++i ) {
    
            Context::Process ();
            this->mAnalysis.Update ();
            this->Scenario_Report ( i );
        }
    }
    
    //----------------------------------------------------------------//
    virtual void Scenario_Report ( int i ) {
        printf ( "ROUND: %d - ", i );
        this->mAnalysis.Print ( false, 1 );
        Context::Print ();
        printf ( "\n" );
    }
    
    //----------------------------------------------------------------//
    virtual bool    Scenario_Control                        ( int step ) = 0;
};

//================================================================//
// CarefulScenario
//================================================================//
class CarefulScenario :
    public Scenario {

    Cohort      mNormal;

public:

    //----------------------------------------------------------------//
    CarefulScenario () {
    
        Context::Reset ();
        Context::InitPlayers ( 32 );
        Context::ApplyCohort ( this->mNormal, "NORM", 0, 32 );
        
        this->mNormal.SetStepStyle ( Player::STEP_CAREFUL );
    }

    //================================================================//
    // Scenario
    //================================================================//

    //----------------------------------------------------------------//
    bool Scenario_Control ( int step ) {
        
        //return step < 32;
        return true;
    }
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
    
        Context::Reset ();
        Context::SetScoreRandomizer ( true );
        Context::InitPlayers ( 16 );
        
        Context::ApplyCohort ( this->mRogue, "RGUE", 0, 4 );
        this->mRogue.SetFlags ( 1, 1, 3 );
        this->mRogue.SetFrequency ( 16 );
        
        Context::ApplyCohort ( this->mNormal, "NORM", 4, 12 );
        this->mNormal.SetFlags ( 2, 3, 3 );
        
        //Context::SetPlayerVerbose ( 4, true );
    }

    //================================================================//
    // Scenario
    //================================================================//

    //----------------------------------------------------------------//
    bool Scenario_Control ( int step ) {
        
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
    
        Context::Reset ();
        Context::InitPlayers ( 2 );
        Context::ApplyCohort ( this->mNormal, "NORM", 0, 1 );
        Context::ApplyCohort ( this->mLate, "LATE", 1, 1 );
        
        //this->mNormal.SetVerbose ( true );
        //this->mLate.SetVerbose ( true );
    
        this->mLate.Pause ( true );
    }

    //================================================================//
    // Scenario
    //================================================================//

    //----------------------------------------------------------------//
    bool Scenario_Control ( int step ) {
        
        if ( step == 16 ) {
            this->mLate.Pause ( false );
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
    
        Context::Reset ();
        Context::SetThreshold ( 0.3 );
        Context::InitPlayers ( 4 );
        Context::ApplyCohort ( this->mNormal, "NORM", 0, 3 );
        Context::ApplyCohort ( this->mLate, "LATE", 3, 1 );
    
        this->mLate.Pause ( true );
    }

    //================================================================//
    // Scenario
    //================================================================//

    //----------------------------------------------------------------//
    bool Scenario_Control ( int step ) {
        
        if ( step == 64 ) {
            this->mLate.Pause ( false );
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
    
        Context::Reset ();
        Context::InitPlayers ( 32 );
        
        Context::ApplyCohort ( this->mRandFreq, "RANDFREQ", 0, 32 );
        this->mRandFreq.RandomizeFrequencies ( 10 );
    }

    //================================================================//
    // Scenario
    //================================================================//

    //----------------------------------------------------------------//
    bool Scenario_Control ( int step ) {
    
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
    
        Context::Reset ();
        Context::InitPlayers ( 16 );
        Context::SetDropRate ( 0.8 );
        
        Context::ApplyCohort ( this->mRogue, "RGUE", 0, 4 );
        this->mRogue.SetFlags ( 1, 3, 3 );
        this->mRogue.SetFrequency ( 16 );
        
        Context::ApplyCohort ( this->mNormal, "NORM", 4, 12 );
        this->mNormal.SetFlags ( 2, 3, 3 );
    }

    //================================================================//
    // Scenario
    //================================================================//

    //----------------------------------------------------------------//
    bool Scenario_Control ( int step ) {
    
        switch ( step ) {
            case 0:
                this->mRogue.SetFlags ( 1, 1, 1 );
                break;
            
            case 32:
                this->mRogue.SetFlags ( 1, 3, 3 );
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
    
        Context::Reset ();
        Context::InitPlayers ( 16 );
        Context::ApplyCohort ( this->mNormal, "NORM", 0, 16 );
        
        //this->mNormal.SetVerbose ( true );
    }

    //================================================================//
    // Scenario
    //================================================================//

    //----------------------------------------------------------------//
    bool Scenario_Control ( int step ) {
        
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
    
        Context::Reset ();
        Context::InitPlayers ( 16 );
        
        Context::ApplyCohort ( this->mSleepy, "SLPY", 0, 12 );
        Context::ApplyCohort ( this->mNormal, "NORM", 12, 4 );
    }

    //================================================================//
    // Scenario
    //================================================================//

    //----------------------------------------------------------------//
    bool Scenario_Control ( int step ) {
    
        switch ( step ) {
            case 0:
                this->mSleepy.Pause ( true );
                break;
            
            case 16:
                this->mSleepy.Pause ( false );
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
    
        Context::Reset ();
        Context::InitPlayers ( 4 );
        Context::ApplyCohort ( this->mNormal, "NORM", 0, 4 );
        
        //this->mNormal.SetVerbose ( true );
    }

    //================================================================//
    // Scenario
    //================================================================//

    //----------------------------------------------------------------//
    bool Scenario_Control ( int step ) {
        
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
    
        Context::Reset ();
        Context::InitPlayers ( 10000 );
        Context::ApplyCohort ( this->mNormal, "NORM", 0, 10000 );
    }

    //================================================================//
    // Scenario
    //================================================================//

    //----------------------------------------------------------------//
    bool Scenario_Control ( int step ) {
        
        return true;
    }
    
    //----------------------------------------------------------------//
    void Scenario_Report ( int i ) {
        printf ( "ROUND: %d - ", i );
        Context::PrintTree ( false, 1 );
    }
};

#endif
