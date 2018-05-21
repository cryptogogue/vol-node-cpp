//
//  main.cpp
//  consensus
//
//  Created by Patrick Meehan on 4/27/18.
//  Copyright © 2018 Patrick Meehan. All rights reserved.
//

#include "context.h"
#include "player.h"

static int					sBlockCount			= 0;
static vector < Player >	sPlayers;
static map < int, int >		sEntropy;
float						sDropRate			= 0.0;
int							sCyclesPerStep		= 0;
bool						sRandomizeScore		= false;
float						sThreshold			= 0.75;

//----------------------------------------------------------------//
void print_indent ( int indent ) {

	for ( int i = 0; i < indent; ++i ) {
		printf ( ".   " );
	}
}

//================================================================//
// Tree
//================================================================//
class Tree {
private:

	friend class TreeSummary;

	int					mPlayer;
	map < int, Tree >	mChildren;

public:

	//----------------------------------------------------------------//
	void AddChain ( const Chain& chain ) {

		Tree* cursor = this;

		for ( size_t i = 0; i < chain.mCycles.size (); ++i ) {
			const Cycle& cycle = chain.mCycles [ i ];
			
			for ( size_t j = 0; j < cycle.mChain.size (); ++j ) {
				
				int playerID		= cycle.mChain [ j ];
				cursor				= &cursor->mChildren [ playerID ];
				cursor->mPlayer		= playerID;
			}
		}
	}
	
	//----------------------------------------------------------------//
	Tree () :
		mPlayer ( -1 ) {
	}
};

//================================================================//
// TreeLevelStats
//================================================================//
class TreeLevelStats {
public:

	size_t		mBranches;
	size_t		mContribution;
	
	//----------------------------------------------------------------//
	TreeLevelStats () :
		mBranches ( 0 ),
		mContribution ( 0 ) {
	}
};

//================================================================//
// TreeSummary
//================================================================//
class TreeSummary {
private:

	vector < int >			mPlayers;
	list < TreeSummary >	mChildren;
	
	size_t					mBranches;
	size_t					mContribution;
	size_t					mSubtreeSize;
	float					mPercentOfTotal;

	//----------------------------------------------------------------//
	void ComputePercents ( size_t totalBlocks ) {
	
		this->mPercentOfTotal = totalBlocks > 0 ? (( float )this->mContribution / ( float )totalBlocks ) : 0.0;
		
		list < TreeSummary >::iterator childrenIt = this->mChildren.begin ();
		for ( ; childrenIt != this->mChildren.end (); ++ childrenIt ) {
			childrenIt->ComputePercents ( totalBlocks );
		}
	}
	
	//----------------------------------------------------------------//
	size_t ComputeSize () {
		
		this->mBranches = 0;
		this->mSubtreeSize = 0;
		
		if ( this->mChildren.size ()) {
			list < TreeSummary >::iterator childrenIt = this->mChildren.begin ();
			for ( ; childrenIt != this->mChildren.end (); ++ childrenIt ) {
				TreeSummary& child = *childrenIt;
				child.ComputeSize ();
				this->mSubtreeSize += child.mSubtreeSize;
				this->mBranches += child.mBranches;
			}
			this->mContribution = this->mPlayers.size () * this->mBranches;
			this->mSubtreeSize += this->mContribution;
		}
		else {
			this->mBranches = 1;
			this->mSubtreeSize = this->mPlayers.size ();
			this->mContribution = this->mSubtreeSize;
		}
		return this->mSubtreeSize;
	}

	//----------------------------------------------------------------//
	void SummarizeRecurse ( const Tree& tree ) {
	
		size_t nChildren = tree.mChildren.size ();
	
		if ( nChildren ) {
	
			map < int, Tree >::const_iterator childrenIt = tree.mChildren.begin ();
			
			if ( nChildren == 1 ) {
				this->mPlayers.push_back ( childrenIt->second.mPlayer );
				this->SummarizeRecurse ( childrenIt->second );
			}
			else {

				for ( ; childrenIt != tree.mChildren.end (); ++childrenIt ) {
					this->mChildren.push_back ( TreeSummary ());
					TreeSummary& childSummary = this->mChildren.back ();
					childSummary.mPlayers.push_back ( childrenIt->second.mPlayer );
					childSummary.SummarizeRecurse ( childrenIt->second );
				}
			}
		}
	}

public:
	
	//----------------------------------------------------------------//
	void AnalyzeLevels ( map < size_t, TreeLevelStats >& levels, size_t depth = 0 ) const {
	
		TreeLevelStats& stats = levels [ depth ];
		stats.mBranches += this->mBranches;
		stats.mContribution += this->mContribution;
		
		list < TreeSummary >::const_iterator childrenIt = this->mChildren.begin ();
		for ( ; childrenIt != this->mChildren.end (); ++ childrenIt ) {
			childrenIt->AnalyzeLevels ( levels, depth + 1 );
		}
	}
	
	//----------------------------------------------------------------//
	void Print ( bool verbose, int maxDepth = 0, int depth = 0 ) const {
	
		if (( maxDepth > 0 ) && ( depth >= maxDepth )) return;
	
		size_t nPlayers = this->mPlayers.size ();
	
		print_indent ( depth );
		printf ( "[size: %d, branches: %d, percent: %g]", ( int )nPlayers, ( int )this->mBranches, this->mPercentOfTotal );
		
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
	
	//----------------------------------------------------------------//
	void PrintLevels () const {
	
		size_t totalBlocks = this->mSubtreeSize;
	
		map < size_t, TreeLevelStats > levels;
		this->AnalyzeLevels ( levels );
		
		size_t maxDepth = levels.size ();
		for ( size_t i = 0; i < maxDepth; ++i ) {
			TreeLevelStats& stats = levels [ i ];
			float percent = ( totalBlocks > 0 ) ? (( float )stats.mContribution / ( float )totalBlocks ) : 0.0;
			//printf ( "[branches: %d, percent: %g]", ( int )stats.mBranches, percent );
			printf ( "[%g]", percent );
		}
		printf ( "\n" );
	}
	
	//----------------------------------------------------------------//
	void Summarize ( const Tree& tree ) {
	
		this->SummarizeRecurse ( tree );
		
		size_t totalBlocks = this->ComputeSize ();
		this->ComputePercents ( totalBlocks );
	}
	
	//----------------------------------------------------------------//
	TreeSummary () :
		mBranches ( 0 ),
		mContribution ( 0 ),
		mSubtreeSize ( 0 ),
		mPercentOfTotal ( 0.0 ) {
	}
};

//================================================================//
// Context
//================================================================//

//----------------------------------------------------------------//
void Context::ApplyCohort ( Cohort& cohort, string name, int basePlayer, int size ) {

	cohort.mName		= name;
	cohort.mBasePlayer	= basePlayer;
	
	cohort.mPlayers.clear ();
	for ( int i = 0; i < size; ++i ) {
		int idx = i + basePlayer;
		sPlayers [ idx ].mCohort = &cohort;
		cohort.mPlayers.push_back ( &sPlayers [ idx ]);
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
uint Context::GetScore ( int playerID, int entropy ) {

	if ( sRandomizeScore ) {
		return ( unsigned int )( playerID ^ entropy );
	}
	return ( uint )playerID;
}

//----------------------------------------------------------------//
float Context::GetThreshold () {

	return sThreshold;
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
	summary.Summarize ( tree );
	
	summary.PrintLevels ();
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
	sBlockCount			= 0;
	sDropRate			= 0.0;
	sCyclesPerStep		= 1;
	sRandomizeScore		= false;
	sThreshold			= 0.75;
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

//----------------------------------------------------------------//
void Context::SetScoreRandomizer ( bool randomize ) {

	sRandomizeScore = randomize;
}

//----------------------------------------------------------------//
void Context::SetThreshold ( float threshold ) {

	sThreshold = 1.0 - threshold;
}
