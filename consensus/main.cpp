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
// SleepyScenario
//================================================================//
class SleepyScenario :
	public Scenario {

	Cohort mSlpy;
	Cohort mNorm;

public:

	//----------------------------------------------------------------//
	SleepyScenario () {
	
		Context::Reset ();
		Context::InitPlayers ( 16 );
		Context::SetDropRate ( 0.8 );
		
		this->mSlpy.SetName ( "SLPY" );
		Context::ApplyCohort ( this->mSlpy, 0, 11 );
		
		this->mNorm.SetName ( "NORM" );
		Context::ApplyCohort ( this->mNorm, 12, 15 );
	}

	//================================================================//
	// Scenario
	//================================================================//

	//----------------------------------------------------------------//
	bool Scenario_Control ( int step ) {
	
		switch ( step ) {
			case 0:
				this->mSlpy.Pause ( true );
				break;
			
			case 16:
				this->mSlpy.Pause ( false );
				break;
		}
		return step < 32;
	}
};

//================================================================//
// RogueScenario
//================================================================//
class RogueScenario :
	public Scenario {

	Cohort mRgue;
	Cohort mNorm;

public:

	//----------------------------------------------------------------//
	RogueScenario () {
	
		Context::Reset ();
		Context::InitPlayers ( 16 );
		Context::SetDropRate ( 0.8 );
		
		this->mRgue.SetName ( "RGUE" );
		Context::ApplyCohort ( this->mRgue, 0, 3 );
		this->mRgue.SetFlags ( 3, 3 );
		this->mRgue.SetFrequency ( 16 );
		
		this->mNorm.SetName ( "NORM" );
		Context::ApplyCohort ( this->mNorm, 4, 15 );
		this->mNorm.SetFlags ( 3, 3 );
	}

	//================================================================//
	// Scenario
	//================================================================//

	//----------------------------------------------------------------//
	bool Scenario_Control ( int step ) {
	
		switch ( step ) {
			case 0:
				this->mNorm.SetFlags ( 1, 1 );
				this->mRgue.SetFlags ( 2, 2 );
				break;
			
			case 32:
				this->mNorm.SetFlags ( 3, 3 );
				this->mRgue.SetFlags ( 3, 3 );
				break;
		}
		return step < 64;
	}
};

//================================================================//
// main
//================================================================//

//----------------------------------------------------------------//
int main ( int argc, const char* argv []) {

	RogueScenario scenario;
	scenario.Run ();

	return 0;
}
