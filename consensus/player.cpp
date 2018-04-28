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
// Player
//================================================================//

//----------------------------------------------------------------//
const Player& Player::GetNextPlayerInCycle () {

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
	
	if ( this->mPlayersCheckedCount == nPlayers ) {
		this->mPlayersCheckedMask.clear ();
		this->mPlayersCheckedCount = 0;
	}
	
	return Context::GetPlayer ( playerID );
}

//----------------------------------------------------------------//
void Player::Init ( int playerID ) {

	this->mID = playerID;
	this->mState [ playerID ];
}

//----------------------------------------------------------------//
void Player::Next () {

	const Player& player = this->GetNextPlayerInCycle ();
	
	Chain* myChain = &this->mState [ this->mID ];
	const Chain* otherChain = &player.State ();
	
	const Chain* bestChain = myChain;

	if ( otherChain->GetNextMerit ( this->mID ) > myChain->GetNextMerit ( this->mID )) {
		bestChain = otherChain;
	}
	
	if ( myChain != bestChain ) {
		myChain->CopyFrom ( *bestChain );
	}
	myChain->PushBlock ( this->mID );
}

////----------------------------------------------------------------//
//void Player::Next () {
//
//	const Player& player = this->GetNextPlayerInCycle ();
//	this->mState [ player.mID ].CopyFrom ( player.State ());
//
//	if ( this->mPlayersCheckedCount ) return; // should only be 0 if the cycle is up
//
//	Chain* chain = &this->mState [ this->mID ];
//
//	for ( map < int, Chain >::iterator headIt = this->mState.begin (); headIt != this->mState.end (); ++headIt ) {
//		Chain* test = &headIt->second;
//		if ( test->mBlocks.size () < chain->mBlocks.size ()) return;
//	}
//
//	Chain* bestHead = chain;
//	float bestMerit = chain->GetNextMerit ( this->mID );
//
//	for ( map < int, Chain >::iterator headIt = this->mState.begin (); headIt != this->mState.end (); ++headIt ) {
//		Chain* test = &headIt->second;
//
//		if ( test != chain ) {
//
//			float testMerit = test->GetNextMerit ( this->mID );
//
//			//if (( testPenalty < bestPenalty ) || (( testPenalty == bestPenalty ) && ( test->mBlocks.size () > bestHead->mBlocks.size ()))) {
//			if ( testMerit > bestMerit ) {
//				bestHead		= test;
//				bestMerit		= testMerit;
//			}
//		}
//	}
//
//	if ( bestHead != chain ) {
//		chain->CopyFrom ( *bestHead );
//	}
//	chain->PushBlock ( this->mID );
//}

//----------------------------------------------------------------//
Player::Player () :
	mPlayersCheckedCount ( 0 ),
	mID ( -1 ) {
}

//----------------------------------------------------------------//
void Player::Print () {

	this->mState [ this->mID ].Print ();
}

//----------------------------------------------------------------//
const Chain& Player::State () const {
	
	map < int, Chain >::const_iterator stateIt = this->mState.find ( this->mID );
	assert ( stateIt != this->mState.end ());
	return stateIt->second;
}
