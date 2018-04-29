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
// Block
//================================================================//
	
//----------------------------------------------------------------//
Block::Block () :
	mHeight ( -1 ),
	mPlayerID ( -1 ),
	mBlockID ( -1 ),
	mMerit ( 0.0 ) {
}

//----------------------------------------------------------------//
float Block::GetNextMerit ( int playerID ) const {

	int nPlayers = Context::CountPlayers ();
	map < int, int > playersByScore;

	for ( int i = 0; i < nPlayers; ++i ) {
		int id = Context::GetPlayer ( i ).GetID ();
		playersByScore [ id ^ this->mEntropy ] = id;
	}

	float merit = 0.0;
	for ( map < int, int >::iterator playersByScoreIt = playersByScore.begin (); playersByScoreIt != playersByScore.end (); ++playersByScoreIt ) {
		if ( playersByScoreIt->second == playerID ) {
			break;
		}
		merit += 1.0;
	}

	merit = merit / ( float )nPlayers;
	merit = merit * merit;
	
	return this->mMerit + merit;
}

//================================================================//
// Chain
//================================================================//

//----------------------------------------------------------------//
void Chain::CopyFrom ( const Chain& from ) {
	
	this->mBlocks = from.mBlocks;
	this->mMerit = from.mMerit;
}

//----------------------------------------------------------------//
float Chain::GetNextMerit ( int playerID ) const {

	int trimIndex;
	return this->GetNextMerit ( playerID, trimIndex );
}

//----------------------------------------------------------------//
float Chain::GetNextMerit ( int playerID, int& trimIndex ) const {
	
	float bestMerit = 0.0;
	trimIndex = 0;
	
	if ( this->mBlocks.size ()) {
		
		int nPlayers = Context::CountPlayers ();
		float penaltyStep = 1.0 / ( float )nPlayers;
		vector < float > penalties;
		penalties.resize ( nPlayers, 0.0 );
		
		list < Block >::const_iterator blockIt = this->mBlocks.begin ();
		int bestBlockID = 0;
		
		for ( int i = 0; blockIt != this->mBlocks.end (); ++blockIt, ++i ) {
			const Block& block = *blockIt;
			
			penalties [ block.mPlayerID ] = 1.0;
			
			float merit = block.GetNextMerit ( playerID ) - penalties [ playerID ];
			if ( merit > bestMerit ) {
				bestMerit = merit;
				bestBlockID = i;
			}
			
			for ( int j = 0; j < nPlayers; ++j ) {
				float& penalty = penalties [ j ];
				if ( penalty > 0.0 ) {
					penalty -= penaltyStep;
				}
				if ( penalty < 0.0 ) {
					penalty = 0.0;
				}
			}
		}
		
		trimIndex = bestBlockID + 1;
	}
	return bestMerit;
}

//----------------------------------------------------------------//
Chain::Chain () :
	mMerit ( 0.0 ) {
}

//----------------------------------------------------------------//
void Chain::Print () const {

	int i = 0;
	for ( list < Block >::const_iterator blockIt = this->mBlocks.begin (); blockIt != this->mBlocks.end (); ++i, ++blockIt ) {
		if ( i ) {
			printf ( "," );
		}
		printf ( "%d:%x", blockIt->mPlayerID, blockIt->mBlockID );
	}
	printf ( " [%g]\n", this->mMerit );
}

//----------------------------------------------------------------//
void Chain::PushBlock ( int playerID ) {

	int trimIndex;
	this->mMerit = this->GetNextMerit ( playerID, trimIndex );
	this->mBlocks.resize ( trimIndex );
	
	Block next;
	
	next.mPlayerID	= playerID;
	next.mBlockID	= Context::GetBlockID ();
	next.mMerit		= this->mMerit;
	next.mHeight	= ( int )this->mBlocks.size ();
	next.mEntropy	= Context::Entropy ( next.mHeight ) ^ playerID;
	
	this->mBlocks.push_back ( next );
}

