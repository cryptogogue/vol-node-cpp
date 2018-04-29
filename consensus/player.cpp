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
	mPaused ( false ) {
}

//----------------------------------------------------------------//
void Cohort::Pause ( bool paused ) {

	this->mPaused = paused;
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
	
	return Context::RequestPlayer ( *this, playerID );
}

//----------------------------------------------------------------//
void Player::Init ( int playerID ) {

	this->mID = playerID;
}

//----------------------------------------------------------------//
void Player::Next () {

	if ( this->mCohort ) {
		if ( this->mCohort->mPaused ) return;
	}

	if ( Context::Drop ()) return;

	const Player* player = this->GetNextPlayerInCycle ();
	if ( !player ) return;
	
	Chain* myChain = &this->mChain;
	const Chain* otherChain = &player->GetChain ();
	
	const Chain* bestChain = myChain;

	if ( otherChain->GetNextMerit ( this->mID ) > myChain->GetNextMerit ( this->mID )) {
		bestChain = otherChain;
	}
	
	if ( myChain != bestChain ) {
		myChain->CopyFrom ( *bestChain );
	}
	myChain->PushBlock ( this->mID );
}

//----------------------------------------------------------------//
Player::Player () :
	mPlayersCheckedCount ( 0 ),
	mID ( -1 ),
	mCohort ( 0 ) {
}

//----------------------------------------------------------------//
void Player::Print () const {

	this->mChain.Print ();
}
