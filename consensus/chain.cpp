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
	mMerit ( -1.0 ) {
}

//----------------------------------------------------------------//
//float Block::GetNextMerit ( int playerID ) const {
//
//	int nPlayers = Context::CountPlayers ();
//	map < int, int > playersByScore;
//
//	for ( int i = 0; i < nPlayers; ++i ) {
//		int id = Context::GetPlayer ( i ).GetID ();
//		playersByScore [ id ^ this->mEntropy ] = id;
//	}
//
//	float merit = 0.0;
//	for ( map < int, int >::iterator playersByScoreIt = playersByScore.begin (); playersByScoreIt != playersByScore.end (); ++playersByScoreIt ) {
//		if ( playersByScoreIt->second == playerID ) {
//			break;
//		}
//		merit += 1.0;
//	}
//
//	merit = merit / ( float )nPlayers;
//	merit = merit * merit;
//
//	return this->mMerit + merit;
//}

//================================================================//
// Chain
//================================================================//

//----------------------------------------------------------------//
float Chain::AverageSpan () const {

	map < int, int > playerIdToBlockHeight;

	int spanCount = 0;
	int spanSum = 0;

	int height = 0;
	list < Block >::const_iterator blockIt = this->mBlocks.begin ();
	for ( ; blockIt != this->mBlocks.end (); ++height, ++blockIt ) {
		const Block& block = *blockIt;
		
		map < int, int >::iterator entry = playerIdToBlockHeight.find ( block.mPlayerID );
		if ( entry != playerIdToBlockHeight.end ()) {
		
			spanSum += height - entry->second;
			++spanCount;
		}
		playerIdToBlockHeight [ block.mPlayerID ] = height;
	}
	return spanCount ? (( float )spanSum / ( float )spanCount ) : 0.0;
}

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
	
	int entropy		= 0;
	float boost		= 1.0;
	float merit		= 0.0;
	
	trimIndex = ( int )this->mBlocks.size ();
	
	if ( this->mBlocks.size ()) {
		
		const Block& top = this->mBlocks.back ();
		entropy = top.mEntropy;
		merit = top.mMerit;
		
		float thisBoost = 0.0;
		float boostStep = 0.1;
		
		list < Block >::const_reverse_iterator blockRevIt = this->mBlocks.rbegin ();
		for ( ; ( blockRevIt != this->mBlocks.rend ()) && ( thisBoost < 1.0 ); ++blockRevIt ) {
			const Block& block = *blockRevIt;
			
			if ( block.mPlayerID == playerID ) {
				boost = thisBoost;
				break;
			}
			thisBoost += boostStep;
		}
		boost = ( boost * boost );
	}

	return merit + ( Context::GetPlayerMerit ( entropy, playerID ) * boost );
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

