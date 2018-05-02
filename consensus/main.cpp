//
//  main.cpp
//  consensus
//
//  Created by Patrick Meehan on 4/27/18.
//  Copyright © 2018 Patrick Meehan. All rights reserved.
//

#include "context.h"
#include "player.h"

//================================================================//
// Scenario
//================================================================//
class Scenario {
public:

	//----------------------------------------------------------------//
	void Run () {
	
		for ( int i = 0; this->Scenario_Control ( i ); ++i ) {
	
			Context::Process ();
			
			printf ( "ROUND: %d\n", i );
			//Context::PrintTree ( 2 );
			Context::Print ();
			printf ( "\n" );
		}
	}
	
	//----------------------------------------------------------------//
	virtual bool	Scenario_Control						( int step ) = 0;
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
		Context::InitPlayers ( 16 );
		
		Context::ApplyCohort ( this->mRogue, "RGUE", 0, 3 );
		this->mRogue.SetFlags ( 1, 1, 3 );
		this->mRogue.SetFrequency ( 16 );
		
		Context::ApplyCohort ( this->mNormal, "NORM", 4, 15 );
		this->mNormal.SetFlags ( 2, 3, 3 );
	}

	//================================================================//
	// Scenario
	//================================================================//

	//----------------------------------------------------------------//
	bool Scenario_Control ( int step ) {
		
		return step < 64;
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
		Context::InitPlayers ( 16 );
		Context::SetDropRate ( 0.8 );
		
		Context::ApplyCohort ( this->mRandFreq, "RANDFREQ", 0, 15 );
		this->mRandFreq.RandomizeFrequencies ( 10 );
	}

	//================================================================//
	// Scenario
	//================================================================//

	//----------------------------------------------------------------//
	bool Scenario_Control ( int step ) {
	
		return step < 128;
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
		
		Context::ApplyCohort ( this->mRogue, "RGUE", 0, 3 );
		this->mRogue.SetFlags ( 1, 3, 3 );
		this->mRogue.SetFrequency ( 16 );
		
		Context::ApplyCohort ( this->mNormal, "NORM", 4, 15 );
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
		Context::SetDropRate ( 0.8 );
		
		Context::ApplyCohort ( this->mSleepy, "SLPY", 0, 11 );
		Context::ApplyCohort ( this->mNormal, "NORM", 12, 15 );
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
		
		Context::ApplyCohort ( this->mNormal, "NORM", 0, 3 );
		this->mNormal.SetFrequency ( 1 );
	}

	//================================================================//
	// Scenario
	//================================================================//

	//----------------------------------------------------------------//
	bool Scenario_Control ( int step ) {
		
		return step < 32;
	}
};

//================================================================//
// main
//================================================================//

//----------------------------------------------------------------//
int main ( int argc, const char* argv []) {

	FastGangScenario scenario;
	scenario.Run ();

	return 0;
}
