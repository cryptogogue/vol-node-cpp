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

	return Context::GetScore ( this->mID, entropy );
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

	// start walking down both chains. if the cycles match, merge the
	// participants.
	// once we find a spot that doesn't match, we have to decide which chain
	// is better.
	// if one is locked and one is not, just pick the locked chain and go with that.
	// but if they are both locked... then most participants wins?
	// or do we need most participants by a margin?
	// if both the same, keep the status quo...
	// also: can we detect participants that should be in the chain but aren't? (seems like it)

	if ( Context::Drop ()) return;

	const Player* player = this->RequestPlayer ();
	if ( !player ) {
		this->mChain.Push ( this->mID );
		return;
	}
	
	Chain chain0 = this->mChain;
	Chain chain1 = player->mChain;

	if ( this->mVerbose ) {
		printf ( " player: %d\n", this->mID );
		chain0.Print ( "   CHAIN0: " );
		chain1.Print ( "   CHAIN1: " );
	}

	chain0.Push ( this->mID );
	chain1.Push ( this->mID );

	if ( this->mVerbose ) {
		chain0.Print ( "    NEXT0: " );
		chain1.Print ( "    NEXT1: " );
	}
	
	this->mChain = Chain::Choose ( chain0, chain1 );
	
	if ( this->mVerbose ) {
		this->mChain.Print ( "     BEST: " );
		printf ( "\n" );
	}
}
