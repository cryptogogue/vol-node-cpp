//
//  main.cpp
//  consensus
//
//  Created by Patrick Meehan on 4/27/18.
//  Copyright © 2018 Patrick Meehan. All rights reserved.
//

#include "chain.h"
#include "context.h"
#include "player.h"

//================================================================//
// Player
//================================================================//

//----------------------------------------------------------------//
int Player::GetID () const {
	
	return this->mID;
}

//----------------------------------------------------------------//
uint Player::GetScore ( int entropy ) {

	return Player::GetScore ( this->mID, entropy );
}

//----------------------------------------------------------------//
uint Player::GetScore ( int playerID, int entropy ) {

	return ( unsigned int )( playerID ^ entropy );
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
void Player::SetVerbose ( bool verbose ) {

	this->mVerbose = verbose;
}

//----------------------------------------------------------------//
void Player::Step () {

	if ( Context::Drop ()) return;

	const Player* player = this->RequestPlayer ();
	if ( !player ) return;
	
	const Chain* chain0 = &this->mChain;
	const Chain* chain1 = &player->mChain;

	if ( this->mVerbose ) {
		printf ( " player: %d\n", this->mID );
		chain0->Print ( "   CHAIN0: " );
		chain1->Print ( "   CHAIN1: " );
	}

	Chain nextChain0 = *chain0;
	Chain nextChain1 = *chain1;
	
//	bool inTop0 = nextChain0.InTopCycle ( this->mID );
//	bool inTop1 = nextChain1.InTopCycle ( this->mID );
	
//	if ( inTop0 == inTop1 ) {

		nextChain0.Push ( this->mID );
		nextChain1.Push ( this->mID );
//	}
//	else {
//
//		if ( !inTop0 ) {
//			nextChain0.Push ( this->mID );
//		}
//		if ( !inTop1 ) {
//			nextChain1.Push ( this->mID );
//		}
//	}

	if ( this->mVerbose ) {
		nextChain0.Print ( "    NEXT0: " );
		nextChain1.Print ( "    NEXT1: " );
	}
	
	const Chain& best = Chain::Compare ( nextChain0, nextChain1 );
	
	if ( this->mVerbose ) {
		best.Print ( "     BEST: " );
		printf ( "\n" );
	}
	
	this->mChain = best;
}
