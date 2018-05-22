//
//  main.cpp
//  consensus
//
//  Created by Patrick Meehan on 4/27/18.
//  Copyright © 2018 Patrick Meehan. All rights reserved.
//

#include "chain.h"
#include "cohort.h"
#include "context.h"
#include "player.h"

//================================================================//
// Cohort
//================================================================//

//----------------------------------------------------------------//
Cohort::Cohort () :
	mBasePlayer ( 0 ),
	mIsPaused ( false ),
	mGroupFlags ( -1 ),
	mRequestFlags ( -1 ),
	mRespondFlags ( -1 ) {
}

//----------------------------------------------------------------//
void Cohort::Pause ( bool paused ) {

	this->mIsPaused = paused;
}

//----------------------------------------------------------------//
void Cohort::RandomizeFrequencies ( int max ) {

	list < Player* >::iterator playerIt = this->mPlayers.begin ();
	for ( ; playerIt != this->mPlayers.end (); ++playerIt ) {
		( *playerIt )->mFrequency = ( rand () % max ) + 1;
	}
}

//----------------------------------------------------------------//
void Cohort::SetFlags ( int group, int request, int respond ) {

	this->mGroupFlags		= group;
	this->mRequestFlags		= request;
	this->mRespondFlags		= respond;
}

//----------------------------------------------------------------//
void Cohort::SetFrequency ( int frequency ) {

	list < Player* >::iterator playerIt = this->mPlayers.begin ();
	for ( ; playerIt != this->mPlayers.end (); ++playerIt ) {
		( *playerIt )->mFrequency = frequency;
	}
}

//----------------------------------------------------------------//
void Cohort::SetName ( string name ) {

	this->mName = name;
}

//----------------------------------------------------------------//
void Cohort::SetStepStyle ( Player::StepStyle stepStyle ) {

	list < Player* >::iterator playerIt = this->mPlayers.begin ();
	for ( ; playerIt != this->mPlayers.end (); ++playerIt ) {
		( *playerIt )->SetStepStyle ( stepStyle );
	}
}

//----------------------------------------------------------------//
void Cohort::SetVerbose ( bool verbose ) {

	list < Player* >::iterator playerIt = this->mPlayers.begin ();
	for ( ; playerIt != this->mPlayers.end (); ++playerIt ) {
		( *playerIt )->SetVerbose ( verbose );
	}
}
