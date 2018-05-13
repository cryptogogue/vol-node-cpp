//
//  main.cpp
//  consensus
//
//  Created by Patrick Meehan on 4/27/18.
//  Copyright © 2018 Patrick Meehan. All rights reserved.
//

#include "chain.h"
#include "context.h"

//================================================================//
// Cycle
//================================================================//

//----------------------------------------------------------------//
int	Cycle::Compare ( const Cycle& cycle0, const Cycle& cycle1 ) {

	assert ( cycle0.mEntropy == cycle1.mEntropy );

	size_t size0 = cycle0.mPlayerList.size ();
	size_t size1 = cycle1.mPlayerList.size ();

	size_t minSize = size0 < size1 ? size0 : size1;
	for ( size_t i = 0; i < minSize; ++i ) {
		
		unsigned int score0 = Context::GetPlayerScore ( cycle0.mPlayerList [ i ], cycle0.mEntropy );
		unsigned int score1 = Context::GetPlayerScore ( cycle1.mPlayerList [ i ], cycle1.mEntropy );
		
		if ( score0 != score1 ) {
			return score0 < score1 ? -1 : 1;
		}
	}
	if ( size0 != size1 ) {
		return size0 > size1 ? -1 : 1;
	}
	return 0;
}

//----------------------------------------------------------------//
bool Cycle::Contains ( int playerID ) const {
	
	for ( size_t i = 0; i < this->mPlayerList.size (); ++i ) {
		if ( this->mPlayerList [ i ] == playerID ) return true;
	}
	return false;
}

//----------------------------------------------------------------//
Cycle::Cycle () :
	mCycleCount ( 0 ),
	mEntropy ( Context::Entropy ( 0 )) {
}

//----------------------------------------------------------------//
Cycle::Cycle ( int cycleCount ) :
	mCycleCount ( cycleCount ),
	mEntropy ( Context::Entropy ( cycleCount )) {
}

//----------------------------------------------------------------//
int Cycle::FindPosition ( int playerID ) {

	unsigned int score = Context::GetPlayerScore ( playerID, this->mEntropy );

	int position  = 0;
	for ( size_t i = 0; i < this->mPlayerList.size (); ++i, ++position ) {
	
		unsigned int test = Context::GetPlayerScore ( this->mPlayerList [ i ], this->mEntropy );
		if ( score < test ) break;
	}
	return position;
}

//----------------------------------------------------------------//
void Cycle::Insert ( int playerID ) {

	int position = this->FindPosition ( playerID );
	
	this->mPlayerList.resize ( position );
	this->mPlayerList.push_back ( playerID );
}

//----------------------------------------------------------------//
void Cycle::Print () const {

	printf ( "[" );
	for ( size_t i = 0; i < this->mPlayerList.size (); ++i ) {
		if ( i > 0 ) {
			printf ( "," );
		}
		printf ( "%d", this->mPlayerList [ i ]);
	}
	printf ( "]" );
}

//----------------------------------------------------------------//
int Cycle::Size () const {

	return ( int )this->mPlayerList.size ();
}

//================================================================//
// Chain
//================================================================//

//----------------------------------------------------------------//
Chain::Chain () {
}

//----------------------------------------------------------------//
const Chain& Chain::Compare ( const Chain& chain0, const Chain& chain1 ) {

	size_t size0 = chain0.mCycles.size ();
	size_t size1 = chain1.mCycles.size ();

	size_t minSize = size0 < size1 ? size0 : size1;
	for ( size_t i = 0; i < minSize; ++i ) {
		const Cycle& cycle0 = chain0.mCycles [ i ];
		const Cycle& cycle1 = chain1.mCycles [ i ];
		
		int compare = Cycle::Compare ( cycle0, cycle1 );
		
		// prefer the longer cycle
		if ( compare != 0 ) {
			return compare < 0 ? chain0 : chain1;
		}
	}
	
	if ( size0 != size1 ) {
	
		const Chain* shorter		= &chain0;
		const Chain* longer		= &chain1;
		
		if ( size1 < size0 ) {
			shorter		= &chain1;
			longer		= &chain0;
		}
		
		if ( shorter->mCycles.size () == 0 ) return *longer;
	
		const Cycle& shorterTop = shorter->mCycles.back ();
		const Cycle& longerNext = longer->mCycles [ shorterTop.mCycleCount ];
		
		return longerNext.Size () >= shorterTop.Size () ? *longer : *shorter;
	}
	return chain0;
}

//----------------------------------------------------------------//
bool Chain::InTopCycle ( int playerID ) const {

	return this->mCycles.size () ? this->mCycles.back ().Contains ( playerID ) : false;
}

//----------------------------------------------------------------//
Cycle* Chain::GetTop () {

	return this->mCycles.size () > 0 ? &this->mCycles.back () : 0;
}

//----------------------------------------------------------------//
const Cycle* Chain::GetTop () const {

	return this->mCycles.size () > 0 ? &this->mCycles.back () : 0;
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

	for ( size_t i = 0; i < this->mCycles.size (); ++i ) {
		Cycle& cycle = this->mCycles [ i ];
		if ( !cycle.Contains ( playerID )) {
			this->mCycles.resize ( i + 1 );
			cycle.Insert ( playerID );
			return;
		}
	}

	// get the top cycle
	Cycle* cycle = this->GetTop ();

	// if no top or top contains block already, start a new cycle
	if ( !cycle || cycle->Contains ( playerID )) {
		this->mCycles.push_back ( Cycle (( int )this->mCycles.size ()));
		cycle = &this->mCycles.back ();
	}
	cycle->Insert ( playerID );
}
