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
//Block::Block () :
//	mPlayerID ( -1 ),
//	mScore ( 0 ),
//	mParent ( 0 ) {
//}

//================================================================//
// Cycle
//================================================================//

//----------------------------------------------------------------//
int Cycle::Compare ( const Cycle& cycle0, const Cycle& cycle1 ) {

	assert ( cycle0.mEntropy == cycle1.mEntropy );

	size_t size0 = cycle0.mChain.size ();
	size_t size1 = cycle1.mChain.size ();

	size_t minSize = size0 < size1 ? size0 : size1;
	for ( size_t i = 0; i < minSize; ++i ) {
		
		unsigned int score0 = Context::GetScore ( cycle0.mChain [ i ], cycle0.mEntropy );
		unsigned int score1 = Context::GetScore ( cycle1.mChain [ i ], cycle1.mEntropy );
		
		if ( score0 != score1 ) {
			return score0 < score1 ? -1 : 1;
		}
	}
	
	if ( size0 != size1 ) {
		return size0 > size1 ? -1 : 1;
	}
	
	size_t players0 = cycle0.mPlayers.size ();
	size_t players1 = cycle1.mPlayers.size ();
	
	if ( players0 != players1 ) {
		return players0 > players1 ? -1 : 1;
	}
	
	return 0;
}

//----------------------------------------------------------------//
bool Cycle::Contains ( int playerID ) const {
	
	return this->mPlayers.find ( playerID ) != this->mPlayers.end ();
}

//----------------------------------------------------------------//
void Cycle::CopyChain ( const Cycle& cycle ) {

	this->mChain = cycle.mChain;
}

//----------------------------------------------------------------//
size_t Cycle::CountParticipants ( int playerID ) const {

	return this->mPlayers.size () + ( playerID >= 0 ? ( this->Contains ( playerID ) ? 0 : 1 ) : 0 );
}

//----------------------------------------------------------------//
Cycle::Cycle () :
	mCycleID ( 0 ),
	mEntropy ( Context::Entropy ( 0 )),
	mNewPlayerRatio ( 0.0 ) {
}

//----------------------------------------------------------------//
//Block* Cycle::FindBestParent ( Block& block ) {
//
//	Block* cursor = this->mBestBlock;
//	for ( ; cursor; cursor = cursor->mParent ) {
//		if ( cursor->mScore > block.mScore ) return cursor;
//		if ( cursor == &block ) return block.mParent;
//	}
//	return 0;
//}

//----------------------------------------------------------------//
int Cycle::FindPosition ( int playerID ) const {

	unsigned int score = Context::GetScore ( playerID, this->mEntropy );

	int position  = 0;
	for ( size_t i = 0; i < this->mChain.size (); ++i, ++position ) {
	
		unsigned int test = Context::GetScore ( this->mChain [ i ], this->mEntropy );
		if ( score < test ) break;
	}
	return position;
}

//----------------------------------------------------------------//
size_t Cycle::GetLength () {

	return this->mChain.size ();
}

//----------------------------------------------------------------//
bool Cycle::Improve ( int playerID ) {

	bool didImprove = false;
	
	if ( !this->Contains ( playerID )) {
	
		this->mPlayers.insert ( playerID );
		didImprove = true;
	}
	
	if ( !this->IsInChain ( playerID )) {
	
		int position = this->FindPosition ( playerID );
	
		this->mChain.resize ( position );
		this->mChain.push_back ( playerID );
	
		didImprove = true;
	}
	
	return didImprove;
}

//----------------------------------------------------------------//
bool Cycle::IsInChain ( int playerID ) const {

	for ( size_t i = 0; i < this->mChain.size (); ++i ) {
		if ( this->mChain [ i ] == playerID ) return true;
	}
	return false;
}

//----------------------------------------------------------------//
void Cycle::MergeParticipants ( const Cycle& cycle ) {

	set < int >::const_iterator cyclePlayerIt = cycle.mPlayers.cbegin ();
	for ( ; cyclePlayerIt != cycle.mPlayers.cend (); ++ cyclePlayerIt ) {
		int playerID = *cyclePlayerIt;
		if ( !this->Contains ( playerID )) {
			this->mPlayers.insert ( playerID );
		}
	}
}

//----------------------------------------------------------------//
void Cycle::Print () const {

	printf ( "[" );
	for ( size_t i = 0; i < this->mChain.size (); ++i ) {
		if ( i > 0 ) {
			printf ( "," );
		}
		printf ( "%d", this->mChain [ i ]);
	}
	//printf ( ",(%d,%g)]", ( int )this->mPlayers.size (), this->mNewPlayerRatio );
	printf ( " (%d)]", ( int )this->mPlayers.size ());
}

//----------------------------------------------------------------//
void Cycle::SetID ( int cycleID ) {

	this->mCycleID = cycleID;
	this->mEntropy = Context::Entropy ( cycleID );
}

//----------------------------------------------------------------//
//void Cycle::PrintRecurse ( const Block* block ) const {
//
//	if ( block ) {
//		if ( block->mParent ) {
//			this->PrintRecurse ( block->mParent );
//			printf ( "," );
//		}
//		printf ( "%d", block->mPlayerID );
//	}
//}

//----------------------------------------------------------------//
//void Cycle::Update () {
//
//	this->mBestBlock = 0;
//	this->mBestLength = 0;
//	
//	Block* bestChild = 0;
//	do {
//		
//		map < int, Block >::iterator blockIt = this->mBlocks.begin ();
//		for ( ; blockIt != this->mBlocks.end (); ++blockIt ) {
//			Block& child = blockIt->second;
//			if ( child.mParent == this->mBestBlock ) {
//				if ( !bestChild || ( child.mScore > bestChild->mScore )) {
//					bestChild = &child;
//				}
//			}
//		}
//		if ( bestChild ) {
//			this->mBestBlock = bestChild;
//			this->mBestLength++;
//		}
//	}
//	while ( bestChild );
//}

//----------------------------------------------------------------//
void Cycle::UpdatePlayerRatio ( size_t prevCount ) {

	this->mNewPlayerRatio = prevCount > 0 ? ( float )this->CountParticipants () / ( float )prevCount : 1.0;
}

//================================================================//
// Chain
//================================================================//

//----------------------------------------------------------------//
bool Chain::CanEdit ( size_t cycleID, int playerID ) const {

	assert ( cycleID < this->mCycles.size ());
	if ( cycleID == this->mCycles.back ().mCycleID ) return true;
	
	const Cycle& cycle0		= this->mCycles [ cycleID ];
	const Cycle& cycle1		= this->mCycles [ cycleID + 1 ];

	size_t size0			= cycle0.CountParticipants ( playerID );
	size_t size1			= cycle1.CountParticipants ();

	float ratio				= ( float )size1 / ( float )size0;

	return ratio <= Context::GetThreshold ();
}

//----------------------------------------------------------------//
bool Chain::CanEdit ( size_t cycleID, const Chain& chain ) const {

	if ( this->CanEdit ( cycleID )) return true;
	
	size_t participants0 = this->mCycles [ cycleID ].mPlayers.size ();
	size_t participants1 = chain.mCycles [ cycleID ].mPlayers.size ();
	
	if ( participants0 < participants1 ) {
	
		float ratio = ( float )participants0 / ( float )participants1;
		if ( ratio <= Context::GetThreshold ()) return true;
	}
	
	size_t size0 = this->mCycles.size ();
	size_t size1 = chain.mCycles.size ();
	
	size_t min = ( size0 < size1 ) ? size0 : size1;
	
	for ( size_t i = cycleID + 1; i < ( min - 1 ); ++i ) {
		if ( this->mCycles [ i ].CountParticipants () < chain.mCycles [ i ].CountParticipants ()) {
			return true;
		}
	}
	return false;
}

//----------------------------------------------------------------//
Chain::Chain () {
}

//----------------------------------------------------------------//
const Chain& Chain::Choose ( const Chain& chain0, const Chain& chain1 ) {

	size_t size0 = chain0.mCycles.size ();
	size_t size1 = chain1.mCycles.size ();

	size_t minSize = size0 < size1 ? size0 : size1;
	for ( size_t i = 0; i < minSize; ++i ) {
	
		const Cycle& cycle0 = chain0.mCycles [ i ];
		const Cycle& cycle1 = chain1.mCycles [ i ];

		int compare = Cycle::Compare ( cycle0, cycle1 );

		// if cycle0 is better
		if ( compare == -1 ) {
			return Chain::Choose ( i, chain0, chain1 );
		}
		
		// if cycle1 is better
		if ( compare == 1 ) {
			return Chain::Choose ( i, chain1, chain0 );
		}
	}
	
	return chain0;
}

//----------------------------------------------------------------//
const Chain& Chain::Choose ( size_t cycleID, const Chain& prefer, const Chain& other ) {

	// if other is editable, the decision is easy. go with the preferred.
	if ( other.CanEdit ( cycleID )) return prefer;
	
	// other is not editable, which means a critical mass of players have committed
	// to the next cycle. so the only way we can beat it is if we have a critical
	// mass of players later in the preferred chain. (can this happen?)

	size_t max0 = prefer.FindMax ( cycleID );
	size_t max1 = other.FindMax ( cycleID );

	if ( max0 == max1 ) return prefer;

	if ( max1 < max0 ) {
		float ratio = ( float )max1 / ( float )max0;
		if ( ratio <= Context::GetThreshold ()) return prefer;
	}

	// other chain wins it.
	return other;
}

//----------------------------------------------------------------//
//const Chain& Chain::Compare ( const Chain& chain0, const Chain& chain1 ) {
//
//	size_t size0 = chain0.mCycles.size ();
//	size_t size1 = chain1.mCycles.size ();
//
//	size_t minSize = size0 < size1 ? size0 : size1;
//	for ( size_t i = 0; i < minSize; ++i ) {
//		const Cycle& cycle0 = chain0.mCycles [ i ];
//		const Cycle& cycle1 = chain1.mCycles [ i ];
//
//		int compare = Cycle::Compare ( cycle0, cycle1 );
//
//		// prefer the longer cycle
//		if ( compare != 0 ) {
//			return compare < 0 ? chain0 : chain1;
//		}
//	}
//
//	if ( size0 != size1 ) {
//
//		const Chain* shorter		= &chain0;
//		const Chain* longer		= &chain1;
//
//		if ( size1 < size0 ) {
//			shorter		= &chain1;
//			longer		= &chain0;
//		}
//
//		if ( shorter->mCycles.size () == 0 ) return *longer;
//
//		const Cycle& shorterTop = shorter->mCycles.back ();
//		const Cycle& longerNext = longer->mCycles [ shorterTop.mCycleID ];
//
//		return longerNext.Size () >= shorterTop.Size () ? *longer : *shorter;
//	}
//	return chain0;
//}

//----------------------------------------------------------------//
size_t Chain::FindMax ( size_t cycleID ) const {

	size_t max = this->mCycles [ cycleID ].mPlayers.size ();

	size_t size = this->mCycles.size ();
	for ( size_t i = cycleID + 1; i < size; ++i ) {
	
		size_t test = this->mCycles [ i ].mPlayers.size ();
		if ( max < test ) {
			max = test;
		}
	}
	return max;
}

//----------------------------------------------------------------//
Cycle* Chain::GetTopCycle () {

	return this->mCycles.size () > 0 ? &this->mCycles.back () : 0;
}

//----------------------------------------------------------------//
const Cycle* Chain::GetTopCycle () const {

	return this->mCycles.size () > 0 ? &this->mCycles.back () : 0;
}

//----------------------------------------------------------------//
//void Chain::Insert ( int cycleID, int playerID ) {
//
//	this->mCycles.resize ( cycleID + 1 );
//	Cycle& cycle = this->mCycles [ cycleID ];
//	cycle.Insert ( playerID );
//
//	size_t prevCount = cycleID > 0 ? this->mCycles [ cycleID - 1 ].mChain.size () : 0;
//	cycle.UpdatePlayerRatio ( prevCount);
//}

//----------------------------------------------------------------//
void Chain::Knit ( const Chain& chain ) {


	size_t size0 = this->mCycles.size ();
	size_t size1 = chain.mCycles.size ();

	size_t minSize = size0 < size1 ? size0 : size1;
	for ( size_t i = 0; i < minSize; ++i ) {
	
		Cycle& cycle0			= this->mCycles [ i ];
		const Cycle& cycle1		= chain.mCycles [ i ];

		int compare = Cycle::Compare ( cycle0, cycle1 );

		// merge participants *after* compare
		cycle0.MergeParticipants ( cycle1 );

		// if cycle0 is better
		if (( compare == -1 ) && chain.CanEdit ( i, *this )) {
			size1 = i + 1; // truncate chain1 (not really)
			break;
		}
		
		// if cycle1 is better
		if (( compare == 1 ) && this->CanEdit ( i, chain )) {
			size0 = i + 1; // truncate chain0
			this->mCycles.resize ( size0 ); // (really!)
			cycle0.CopyChain ( cycle1 ); // copy over the better chain
			cycle0.mPlayers = cycle1.mPlayers;
			break;
		}
	}
	
	// if size1 is longer, then all the cycles were equal, *or* we found a better cycle in
	// chain1 and truncated chain0. either way, copy the rest of chain1 over to chain0.
	for ( size_t i = size0; i < size1; ++i ) {
		this->mCycles.push_back ( chain.mCycles [ i ]);
	}
}

//----------------------------------------------------------------//
void Chain::MergeParticipants ( const Chain& chain ) {

	size_t size0 = this->mCycles.size ();
	size_t size1 = chain.mCycles.size ();

	size_t minSize = size0 < size1 ? size0 : size1;
	for ( size_t i = 0; i < minSize; ++i ) {

		this->mCycles [ i ].MergeParticipants ( chain.mCycles [ i ]);
	}
}

//----------------------------------------------------------------//
void Chain::Print ( const char* pre, const char* post ) const {

	if ( pre ) {
		printf ( "%s", pre );
	}

	for ( size_t i = 0; i < this->mCycles.size (); ++i ) {
		this->mCycles [ i ].Print ();
	}
	
	if ( post ) {
		printf ( "%s", post );
	}
}

//----------------------------------------------------------------//
void Chain::Push ( int playerID ) {

	// first, seek back to find the earliest cycle we could change.
	// we're only allowed change if next cycle ratio is below the threshold.
	// important to measure the threshold *after* the proposed change.
	size_t nCycles = this->mCycles.size ();
	if ( nCycles > 0 ) {
	
		// start at the last cycle and count backward until we find a cycle we
		// aren't allowed to change. we want the cycle after that (which may be
		// the last cycle in the chain).
		size_t baseCycleID = nCycles - 1;
		for ( ; baseCycleID > 0; --baseCycleID ) {
			if ( !this->CanEdit ( baseCycleID - 1, playerID )) break;
		}
		
		// now count forward from the base until we find a cycle that we'd want to change.
		// again, base may be the last cycle in the chain.
		// we may not find one, in which case we need to add a new cycle.
		for ( size_t i = baseCycleID; i < nCycles; ++i ) {
			Cycle& cycle = this->mCycles [ i ];
			if ( cycle.Improve ( playerID )) {
				this->mCycles.resize ( i + 1 );
				return;
			}
		}
	}

	// add a new cycle.
	int cycleID = ( int )this->mCycles.size ();
	this->mCycles.push_back ( Cycle ());
	Cycle* cycle = this->GetTopCycle ();
	cycle->SetID ( cycleID );
	cycle->Improve ( playerID ); // cycle is empty, so this is guaranteed.
}
