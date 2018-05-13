//
//  main.cpp
//  consensus
//
//  Created by Patrick Meehan on 4/27/18.
//  Copyright © 2018 Patrick Meehan. All rights reserved.
//

#include "analysis.h"
#include "context.h"
#include "player.h"

//================================================================//
// Cohort
//================================================================//

//----------------------------------------------------------------//
Cohort::Cohort () :
	mBasePlayer ( -1 ),
	mTopPlayer ( -1 ),
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
void Cohort::SetVerbose ( bool verbose ) {

	list < Player* >::iterator playerIt = this->mPlayers.begin ();
	for ( ; playerIt != this->mPlayers.end (); ++playerIt ) {
		( *playerIt )->SetVerbose ( verbose );
	}
}

//================================================================//
// Player
//================================================================//

//----------------------------------------------------------------//
int Player::GetID () const {
	
	return this->mID;
}

//----------------------------------------------------------------//
const Player* Player::GetNextPlayerInCycle () {

	int nPlayers = Context::CountPlayers ();
	this->mPlayersCheckedMask.resize ( nPlayers, false );
	
	// mark self as checked
	if ( this->mPlayersCheckedCount == 0 ) {
		this->mPlayersCheckedMask [ this->mID ] = true;
		this->mPlayersCheckedCount = 1;
	}
	
	int playerID = 0;
	do {
		playerID = rand () % nPlayers;
	} while ( this->mPlayersCheckedMask [ playerID ]);
	
	this->mPlayersCheckedCount++;
	this->mPlayersCheckedMask [ playerID ] = true;
	
	if ( this->mPlayersCheckedCount >= nPlayers ) {
		this->mPlayersCheckedMask.clear ();
		this->mPlayersCheckedCount = 0;
	}
	
	return &Context::GetPlayer ( playerID );
}

//----------------------------------------------------------------//
void Player::Init ( int playerID ) {

	this->mID = playerID;
}

//----------------------------------------------------------------//
void Player::Next () {

	if ( !this->mCohort ) return;
	if ( this->mCohort->mIsPaused ) return;

	for ( int i = 0; i < this->mFrequency; ++i ) {
		this->Step ();
	}
}

//----------------------------------------------------------------//
Player::Player () :
	mPlayersCheckedCount ( 0 ),
	mID ( -1 ),
	mCohort ( 0 ),
	mFrequency ( 1 ),
	mVerbose ( false ) {
}

//----------------------------------------------------------------//
void Player::Print () const {

	printf ( "[%s] ", this->mCohort ? this->mCohort->mName.c_str () : "" );
	this->mAnalysis.Print ();
}

//----------------------------------------------------------------//
const Player* Player::RequestPlayer () {

	const Player* player = 0;
	
	do {
		player = this->GetNextPlayerInCycle ();
	} while ( !(( this->mCohort->mRequestFlags & player->mCohort->mGroupFlags ) && ( player->mCohort->mRespondFlags & this->mCohort->mGroupFlags )));
	
	return player;
}

//----------------------------------------------------------------//
void Player::SetVerbose ( bool verbose ) {

	this->mVerbose = verbose;
}

//----------------------------------------------------------------//
void Player::Step () {

	if ( Context::Drop ()) return;

	const Player* player = this->RequestPlayer ();
	if ( !player ) return;
	
	const Analysis* analysis0 = &this->mAnalysis;
	const Analysis* analysis1 = &player->mAnalysis;

	if ( this->mVerbose ) {
		printf ( " player: %d\n", this->mID );
		analysis0->Print		( "   CHAIN0: " );
		analysis1->Print		( "   CHAIN1: " );
	}

	Analysis nextAnalysis0 = *analysis0;
	Analysis nextAnalysis1 = *analysis1;
	
	bool inTop0 = nextAnalysis0.InTopCycle ( this->mID );
	bool inTop1 = nextAnalysis1.InTopCycle ( this->mID );
	
	if ( inTop0 == inTop1 ) {

		nextAnalysis0.Push ( this->mID );
		nextAnalysis1.Push ( this->mID );
	}
	else {

		if ( !inTop0 ) {
			nextAnalysis0.Push ( this->mID );
		}
		if ( !inTop1 ) {
			nextAnalysis1.Push ( this->mID );
		}
	}

	if ( this->mVerbose ) {
		nextAnalysis0.Print		( "    NEXT0: " );
		nextAnalysis1.Print		( "    NEXT1: " );
	}
	
	const Analysis& best = Analysis::Compare ( nextAnalysis0, nextAnalysis1 );
	
	if ( this->mVerbose ) {
		best.Print				( "     BEST: " );
		printf ( "\n" );
	}
	
	this->mAnalysis = best;
}
