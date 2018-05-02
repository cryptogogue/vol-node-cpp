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

//================================================================//
// Player
//================================================================//

//----------------------------------------------------------------//
const Chain& Player::GetChain () const {
	
	return this->mChain;
}

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
	mFrequency ( 1 ) {
}

//----------------------------------------------------------------//
void Player::Print () const {

	printf ( "[%s] ", this->mCohort ? this->mCohort->mName.c_str () : "" );
	this->mChain.Print ();
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
void Player::Step () {

	if ( Context::Drop ()) return;

	const Player* player = this->RequestPlayer ();
	if ( !player ) return;
	
	float testMerit				= 0.0;
	float bestMerit				= 0.0;
	const Chain* bestChain		= 0;
	bool push					= false;
	
	Chain* chain0				= &this->mChain;
	const Chain* chain1			= &player->GetChain ();
	
	float lengthDiff = ( float )chain1->mBlocks.size () - ( float )chain0->mBlocks.size ();
	lengthDiff = lengthDiff < 0 ? -lengthDiff : lengthDiff;
	lengthDiff = lengthDiff / 100.0;
	lengthDiff = lengthDiff * lengthDiff;
	
	float diversity0			= lengthDiff > 0.0 ? ( chain0->AverageSpan () * lengthDiff ) : 0.0;
	float diversity1			= lengthDiff > 0.0 ? ( chain1->AverageSpan () * lengthDiff ) : 0.0;
	
//	bestMerit = chain0->GetNextMerit ( this->mID ) + diversity0;
//	bestChain = chain0;
//
//	testMerit = chain1->GetNextMerit ( this->mID ) + diversity1;
//	if ( bestMerit < testMerit ) {
//		bestMerit = testMerit;
//		bestChain = chain1;
//	}
//
//	if ( bestChain != &this->mChain ) {
//		this->mChain.CopyFrom ( *bestChain );
//	}
//	this->mChain.PushBlock ( this->mID );
	
	// chain 0, as is
	bestMerit = chain0->mMerit + diversity0;
	bestChain = chain0;

	// chain 1, as is
	testMerit = chain1->mMerit + diversity1;
	if ( bestMerit < testMerit ) {
		bestMerit = testMerit;
		bestChain = chain1;
	}

	// chain 0, with push
	testMerit = chain0->GetNextMerit ( this->mID ) + diversity0;
	if ( bestMerit < testMerit ) {
		bestMerit = testMerit;
		bestChain = chain0;
		push = true;
	}

	// chain 1, with push
	testMerit = chain1->GetNextMerit ( this->mID ) + diversity1;
	if ( bestMerit < testMerit ) {
		bestMerit = testMerit;
		bestChain = chain1;
		push = true;
	}

	if ( bestChain != &this->mChain ) {
		this->mChain.CopyFrom ( *bestChain );
	}

	if ( push ) {
		this->mChain.PushBlock ( this->mID );
	}
}
