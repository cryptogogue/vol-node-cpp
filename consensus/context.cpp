//
//  main.cpp
//  consensus
//
//  Created by Patrick Meehan on 4/27/18.
//  Copyright © 2018 Patrick Meehan. All rights reserved.
//

#include "context.h"
#include "player.h"

static int					sBlockCount = 0;
static vector < Player >	sPlayers;
static map < int, int >		sEntropy;
float						sDropRate = 0.0;
int							sCyclesPerStep = 0;

//----------------------------------------------------------------//
void print_indent ( int indent ) {

	for ( int i = 0; i < indent; ++i ) {
		printf ( ".   " );
	}
}

//================================================================//
// TreeSummary
//================================================================//
class TreeSummary {
public:

	vector < int >			mPlayers;
	list < TreeSummary >	mChildren;
	
	//----------------------------------------------------------------//
	void Print ( bool verbose, int maxDepth = 0, int depth = 0 ) const {
	
		if (( maxDepth > 0 ) && ( depth >= maxDepth )) return;
	
		size_t nPlayers = this->mPlayers.size ();
	
		print_indent ( depth );
		printf ( "[size: %d]", ( int )nPlayers );
		
		if ( verbose && ( nPlayers > 0 )) {
			printf ( " - " );
			for ( size_t i = 0; i < nPlayers; ++i ) {
				if ( i > 0 ) {
					printf ( "," );
				}
				printf ( "%d", this->mPlayers [ i ]);
			}
		}
		printf ( "\n" );
		
		++depth;
		list < TreeSummary >::const_iterator childrenIt = this->mChildren.begin ();
		for ( ; childrenIt != this->mChildren.end (); ++ childrenIt ) {
			childrenIt->Print ( verbose, maxDepth, depth );
		}
	}
};

//================================================================//
// Tree
//================================================================//
class Tree {
public:
	
	int					mPlayer;
	map < int, Tree >	mChildren;

	//----------------------------------------------------------------//
	void AddChain ( const Chain& chain ) {

		Tree* cursor = this;

		for ( size_t i = 0; i < chain.mCycles.size (); ++i ) {
			const Cycle& cycle = chain.mCycles [ i ];
			
			for ( size_t j = 0; j < cycle.mPlayerList.size (); ++j ) {
				
				int playerID		= cycle.mPlayerList [ j ];
				cursor				= &cursor->mChildren [ playerID ];
				cursor->mPlayer		= playerID;
			}
		}
	}
	
	//----------------------------------------------------------------//
	void Summarize ( TreeSummary& summary ) const {
	
		int nChildren = ( int )this->mChildren.size ();
	
		if ( nChildren ) {
	
			map < int, Tree >::const_iterator childrenIt = this->mChildren.begin ();
			
			if ( this->mChildren.size () == 1 ) {
				summary.mPlayers.push_back ( childrenIt->second.mPlayer );
				childrenIt->second.Summarize ( summary );
			}
			else {

				for ( ; childrenIt != this->mChildren.end (); ++childrenIt ) {
					summary.mChildren.push_back ( TreeSummary ());
					TreeSummary& childSummary = summary.mChildren.back ();
					childSummary.mPlayers.push_back ( childrenIt->second.mPlayer );
					childrenIt->second.Summarize ( childSummary );
				}
			}
		}
	}
};

//================================================================//
// Context
//================================================================//

//----------------------------------------------------------------//
void Context::ApplyCohort ( Cohort& cohort, string name, int basePlayer, int topPlayer ) {

	cohort.mName		= name;
	cohort.mBasePlayer	= basePlayer;
	cohort.mTopPlayer	= topPlayer;
	
	cohort.mPlayers.clear ();
	for ( int i = basePlayer; i <= topPlayer; ++i ) {
		sPlayers [ i ].mCohort = &cohort;
		cohort.mPlayers.push_back ( &sPlayers [ i ]);
	}
}

//----------------------------------------------------------------//
int Context::CountPlayers () {

	return ( int )sPlayers.size ();
}

//----------------------------------------------------------------//
bool Context::Drop () {

	return (( float )( rand () % ( 1000 + 1 )) / 1000.0 ) < sDropRate;
}

//----------------------------------------------------------------//
int Context::Entropy ( int height ) {

	if ( sEntropy.find ( height ) != sEntropy.end ()) {
		return sEntropy [ height ];
	}
	int entropy = rand ();
	sEntropy [ height ] = entropy;
	return entropy;
}

//----------------------------------------------------------------//
int Context::GetBlockID () {

	return sBlockCount++;
}

//----------------------------------------------------------------//
const Player& Context::GetPlayer ( int playerID ) {

	return sPlayers [ playerID ];
}

//----------------------------------------------------------------//
void Context::InitPlayers ( int nPlayers ) {

	sPlayers.resize ( nPlayers );

	for ( int i = 0; i < nPlayers; ++i ) {
		sPlayers [ i ].Init ( i );
	}
}

//----------------------------------------------------------------//
void Context::Print () {

	int nPlayers = Context::CountPlayers ();
	for ( int i = 0; i < nPlayers; ++i ) {
		Player& player = sPlayers [ i ];
		player.Print ();
	}
}

//----------------------------------------------------------------//
void Context::PrintTree ( bool verbose, int maxDepth ) {

	Tree tree;
	int nPlayers = Context::CountPlayers ();
	for ( int i = 0; i < nPlayers; ++i ) {
		Player& player = sPlayers [ i ];
		tree.AddChain ( player.mChain );
	}
	TreeSummary summary;
	tree.Summarize ( summary );
	summary.Print ( verbose, maxDepth );
}

//----------------------------------------------------------------//
void Context::Process () {

	int nPlayers = Context::CountPlayers ();
	int cycles = sCyclesPerStep ? sCyclesPerStep : nPlayers;
	
	for ( int i = 0; i < cycles; ++i ) {
	
		map < Player*, int > schedule;
		
		for ( int j = 0; j < nPlayers; ++j ) {
			Player& player = sPlayers [ j ];
			schedule [ &player ] = player.mFrequency;
		}
		
		while ( schedule.size ()) {
		
			map < Player*, int >::iterator scheduleIt = next ( schedule.begin (), rand () % schedule.size ());
			
			scheduleIt->first->Next ();
			scheduleIt->second -= 1;
			if ( scheduleIt->second == 0 ) {
				schedule.erase ( scheduleIt );
			}
		}
	}
}

//----------------------------------------------------------------//
void Context::Reset () {

	srand ( 1 );
	sPlayers.clear ();
	sEntropy.clear ();
	sBlockCount = 0;
	sDropRate = 0.0;
	sCyclesPerStep = 1;
}

//----------------------------------------------------------------//
void Context::SetCyclesPerStep ( int cycles ) {

	sCyclesPerStep = cycles;
}

//----------------------------------------------------------------//
void Context::SetDropRate ( float percentage ) {

	sDropRate = percentage;
}

//----------------------------------------------------------------//
void Context::SetPlayerVerbose ( int playerID, bool verbose ) {

	sPlayers [ playerID ].mVerbose = verbose;
}
