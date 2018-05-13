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

	for ( int i = 0; i < ( int )this->mBlocks.size (); ++i ) {
		const Block& block = this->mBlocks [ i ];
		
		map < int, int >::iterator entry = playerIdToBlockHeight.find ( block.mPlayerID );
		if ( entry != playerIdToBlockHeight.end ()) {
		
			spanSum += ( int )i - entry->second;
			++spanCount;
		}
		playerIdToBlockHeight [ block.mPlayerID ] = i;
	}
	return spanCount ? (( float )spanSum / ( float )spanCount ) : 0.0;
}

//----------------------------------------------------------------//
Chain::Chain () :
	mSpanSum ( 0 ),
	mSpanCount ( 0 ),
	mDiversity ( 0.0 ),
	mMerit ( 0.0 ),
	mWidth ( 0.0 ),
	mPenalty ( 0.0 ),
	mScore ( 0.0 ),
	mParticipation ( 0.0 ) {
}

//----------------------------------------------------------------//
void Chain::CopyFrom ( const Chain& from ) {
	
	*this = from;
}

//----------------------------------------------------------------//
float Chain::FindBestScore ( int playerID, int& position ) const {

	int entropy			= 0;
	float merit			= 0.0;
	int					spanSum = 0;
	int					spanCount = 0;
	map < int, int >	playerIdToBlockHeight;

	position			= 0;
	float bestScore 	= 0.0;

	if ( !this->mBlocks.size ()) {
		return Context::GetPlayerMerit ( entropy, playerID );
	}

	for ( int i = 0; i < ( int )this->mBlocks.size (); ++i ) {
		const Block& block = this->mBlocks [ i ];
		
		map < int, int >::const_iterator spanIt = playerIdToBlockHeight.find ( block.mPlayerID );
		if ( spanIt != playerIdToBlockHeight.end ()) {
			spanSum		+= i - spanIt->second;
			spanCount	+= 1;
		}
		playerIdToBlockHeight [ block.mPlayerID ] = i;
		merit += Context::GetPlayerMerit ( entropy, block.mPlayerID );
		entropy = block.mEntropy;
		
		float testMerit		= merit + Context::GetPlayerMerit ( entropy, playerID );
		int testSpanSum		= spanSum;
		int testSpanCount	= spanCount;
		
		spanIt = playerIdToBlockHeight.find ( playerID );
		if ( spanIt != playerIdToBlockHeight.end ()) {
			testSpanSum		+= i - spanIt->second;
			testSpanCount	+= 1;
		}
		
		float testScore = testMerit * ( 1.0 + ( testSpanCount ? (( float )testSpanSum / ( float )testSpanCount ) : 0.0 ));
		
		if ( bestScore < testScore ) {
			bestScore = testScore;
			position = i + 1;
		}
	}
	
	return bestScore;
}

//----------------------------------------------------------------//
float Chain::GetNextDiversity ( int playerID ) const {

	int spanSum		= this->mSpanSum;
	int spanCount	= this->mSpanCount;
	return this->GetNextDiversity ( playerID, spanSum, spanCount );
}

//----------------------------------------------------------------//
float Chain::GetNextDiversity ( int playerID, int& spanSum, int& spanCount ) const {

	map < int, int >::const_iterator entry = this->mPlayerIdToBlockHeight.find ( playerID );
	if ( entry != this->mPlayerIdToBlockHeight.end ()) {
	
		spanSum += ( int )this->mBlocks.size () - entry->second;
		++spanCount;
	}
	return spanCount ? ( float )spanSum / ( float )spanCount : 0.0;
}

//----------------------------------------------------------------//
float Chain::GetNextMerit ( int playerID ) const {
	
	//int entropy		= 0;
	//float boost		= 1.0;
	//float merit		= 0.0;
	
	//trimIndex = ( int )this->mBlocks.size ();
	
//	if ( this->mBlocks.size ()) {
//
//		const Block& top = this->mBlocks.back ();
//		entropy = top.mEntropy;
//		merit = top.mMerit;
//
////		float thisBoost = 0.0;
////		float boostStep = 0.1;
////
////		list < Block >::const_reverse_iterator blockRevIt = this->mBlocks.rbegin ();
////		for ( ; ( blockRevIt != this->mBlocks.rend ()) && ( thisBoost < 1.0 ); ++blockRevIt ) {
////			const Block& block = *blockRevIt;
////
////			if ( block.mPlayerID == playerID ) {
////				boost = thisBoost;
////				break;
////			}
////			thisBoost += boostStep;
////		}
////		boost = ( boost * boost );
//	}

	return this->mMerit + Context::GetPlayerMerit ( Context::Entropy (( int )this->mBlocks.size ()), playerID );
}

//----------------------------------------------------------------//
float Chain::GetNextParticipation ( int playerID ) const {

	int max = 0;
	bool found = false;
	map < int, int >::const_iterator histIt = this->mHistogram.begin ();
	for ( ; histIt != this->mHistogram.end (); ++histIt ) {
	
		int test = histIt->second;
	
		if ( histIt->first == playerID ) {
			found = true;
			test++;
		}
		
		if ( max < test ) {
			max = test;
		}
	}
	
	max = (( max == 0 ) && ( !found )) ? 1 : max;
	
	float norm = 1.0 / ( float )max;
	float sum = 0.0;
	
	histIt = this->mHistogram.begin ();
	for ( ; histIt != this->mHistogram.end (); ++histIt ) {
	
		int count = histIt->second;
		if ( histIt->first == playerID ) {
			count++;
		}
		sum += ( float )count * norm;
	}
	
	//int count = ( int )this->mHistogram.size ();
	if ( !found ) {
		sum += norm;
	}
	
	return sum / ( float )Context::CountPlayers ();
}

//----------------------------------------------------------------//
float Chain::GetNextPenalty ( int playerID ) const {
	
	double beacon	= 0.0;
	double penalty	= 0.0;
	double size		= ( double )this->mBlocks.size () + 1.0;
	
	for ( int i = 0; i < ( int )this->mBlocks.size (); ++i ) {
		const Block& block = this->mBlocks [ i ];
		
		double diff = beacon - (( double )block.mPlayerID / size );
		penalty += diff < 0.0 ? -diff : diff;
		beacon = ( double )block.mEntropy / size;
	}
	
	double p = (( double )playerID / size );
	double diff = beacon - p;
	penalty += diff < 0.0 ? -diff : diff;
	
	return ( float )penalty;
}

//----------------------------------------------------------------//
int Chain::GetNextWidth ( int playerID ) const {

	return this->mWidth + ( this->mPlayerIdToBlockHeight.find ( playerID ) == this->mPlayerIdToBlockHeight.end () ? 1.0 : 0.0 );
}

//----------------------------------------------------------------//
void Chain::InsertBlock ( int playerID, int position, float score ) {

	this->mBlocks.resize ( position );
	
	Block next;
	
	next.mPlayerID	= playerID;
	next.mBlockID	= Context::GetBlockID ();
	next.mEntropy	= Context::Entropy ( next.mHeight ) ^ playerID;
	
	this->mBlocks.push_back ( next );
	
	this->mScore = score;
}

//----------------------------------------------------------------//
bool Chain::IsTop ( int playerID ) const {

	return this->mBlocks.size () ? this->mBlocks.back ().mPlayerID == playerID : false;
}

//----------------------------------------------------------------//
void Chain::Print ( const char* pre, const char* post ) const {

	if ( pre ) {
		printf ( "%s", pre );
	}

	for ( int i = 0; i < ( int )this->mBlocks.size (); ++i ) {
		const Block& block = this->mBlocks [ i ];
		if ( i > 0 ) {
			printf ( "," );
		}
		//printf ( "%d:%x", blockIt->mPlayerID, blockIt->mBlockID );
		printf ( "%d", block.mPlayerID);
	}
	//printf ( " [%g, %g]\n", this->mMerit, this->mMerit / ( float )this->mBlocks.size ());
	//printf ( " [w:%d, d:%g, m:%g, p:%g]\n", ( int )this->mWidth, this->mDiversity, this->mMerit, this->mPenalty );
	//printf ( " [%g]\n", this->mScore );
	printf ( " [%g, %g]", this->mMerit, this->mMerit / ( float )this->mBlocks.size ());
	
	if ( post ) {
		printf ( "%s", post );
	}
}

//----------------------------------------------------------------//
void Chain::PushBlock ( int playerID ) {

	this->mWidth			= this->GetNextWidth ( playerID );
	this->mDiversity		= this->GetNextDiversity ( playerID, this->mSpanSum, this->mSpanCount );
	this->mMerit			= this->GetNextMerit ( playerID );
	this->mPenalty			= this->GetNextPenalty ( playerID );
	this->mParticipation	= this->GetNextParticipation ( playerID );
	
	Block next;
	
	next.mPlayerID	= playerID;
	next.mBlockID	= Context::GetBlockID ();
	next.mMerit		= this->mMerit;
	next.mHeight	= ( int )this->mBlocks.size ();
	next.mEntropy	= Context::Entropy ( next.mHeight ) ^ playerID;
	
	this->mBlocks.push_back ( next );
	this->mPlayerIdToBlockHeight [ playerID ] = next.mHeight;
	
	if ( this->mHistogram.find ( playerID ) == this->mHistogram.end ()) {
		this->mHistogram [ playerID ] = 1;
	}
	else {
		this->mHistogram [ playerID ]++;
	}
}

