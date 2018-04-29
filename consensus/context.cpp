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

	vector < Block >		mBlocks;
	list < TreeSummary >	mChildren;
	
	//----------------------------------------------------------------//
	void Print ( int maxDepth = 0, int depth = 0 ) const {
	
		if (( maxDepth > 0 ) && ( depth >= maxDepth )) return;
	
		print_indent ( depth );
		printf ( "[size: %d]\n", ( int )this->mBlocks.size ());
		
		++depth;
		list < TreeSummary >::const_iterator childrenIt = this->mChildren.begin ();
		for ( ; childrenIt != this->mChildren.end (); ++ childrenIt ) {
			childrenIt->Print ( maxDepth, depth );
		}
	}
};

//================================================================//
// Tree
//================================================================//
class Tree {
public:
	
	Block				mBlock;
	map < int, Tree >	mChildren;

	//----------------------------------------------------------------//
	void AddChain ( const Chain& chain ) {
	
		Tree* cursor = this;
		
		list < Block >::const_iterator blockIt = chain.mBlocks.begin ();
		for ( ; blockIt != chain.mBlocks.end (); ++blockIt ) {
			const Block& block	= *blockIt;
			cursor				= &cursor->mChildren [ block.mBlockID ];
			cursor->mBlock		= block;
		}
	}
	
	//----------------------------------------------------------------//
	void Summarize ( TreeSummary& summary ) const {
	
		int nChildren = ( int )this->mChildren.size ();
	
		if ( nChildren ) {
	
			map < int, Tree >::const_iterator childrenIt = this->mChildren.begin ();
			
			if ( this->mChildren.size () == 1 ) {
				summary.mBlocks.push_back ( childrenIt->second.mBlock );
				childrenIt->second.Summarize ( summary );
			}
			else {

				for ( ; childrenIt != this->mChildren.end (); ++childrenIt ) {
					summary.mChildren.push_back ( TreeSummary ());
					TreeSummary& childSummary = summary.mChildren.back ();
					childSummary.mBlocks.push_back ( childrenIt->second.mBlock );
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
void Context::ApplyCohort ( Cohort& cohort, int basePlayer, int topPlayer ) {

	cohort.mBasePlayer	= basePlayer;
	cohort.mTopPlayer	= topPlayer;

	for ( int i = basePlayer; i <= topPlayer; ++i ) {
		sPlayers [ i ].mCohort = &cohort;
	}
}

//----------------------------------------------------------------//
int Context::CountPlayers () {

	return ( int )sPlayers.size ();
}

//----------------------------------------------------------------//
bool Context::Drop () {

	return (( float )(( rand () % 1000) + 1 ) / 1000.0 ) < sDropRate;
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

	sPlayers.clear ();
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
void Context::PrintTree ( int maxDepth ) {

	Tree tree;
	int nPlayers = Context::CountPlayers ();
	for ( int i = 0; i < nPlayers; ++i ) {
		Player& player = sPlayers [ i ];
		tree.AddChain ( player.GetChain ());
	}
	TreeSummary summary;
	tree.Summarize ( summary );
	summary.Print ( maxDepth );
}

//----------------------------------------------------------------//
void Context::Process ( int cycles ) {

	int nPlayers = Context::CountPlayers ();
	for ( int i = 0; i < cycles; ++i ) {
		for ( int j = 0; j < nPlayers; ++j ) {
			Player& player = sPlayers [ j ];
			player.Next ();
		}
	}
}

//----------------------------------------------------------------//
const Player* Context::RequestPlayer ( Player& requestedBy, int playerID ) {

	return &sPlayers [ playerID ];
}

//----------------------------------------------------------------//
void Context::SetDropRate ( float percentage ) {

	sDropRate = percentage;
}
