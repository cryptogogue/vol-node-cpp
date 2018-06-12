// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <simulator/analysis.h>
#include <simulator/chain.h>
#include <simulator/context.h>
#include <simulator/player.h>

//----------------------------------------------------------------//
void print_indent ( int indent ) {

    for ( int i = 0; i < indent; ++i ) {
        printf ( ".   " );
    }
}

//================================================================//
// Tree
//================================================================//

//----------------------------------------------------------------//
void Tree::AddChain ( const Chain& chain ) {

    Tree* cursor = this;

    for ( size_t i = 0; i < chain.mCycles.size (); ++i ) {
        const Cycle& cycle = chain.mCycles [ i ];
        
        for ( size_t j = 0; j < cycle.mChain.size (); ++j ) {
            
            int playerID        = cycle.mChain [ j ];
            cursor              = &cursor->mChildren [ playerID ];
            cursor->mPlayer     = playerID;
        }
    }
}

//----------------------------------------------------------------//
Tree::Tree () :
    mPlayer ( -1 ) {
}

//================================================================//
// TreeLevelStats
//================================================================//
    
//----------------------------------------------------------------//
TreeLevelStats::TreeLevelStats () :
    mChains ( 0 ),
    mContribution ( 0 ) {
}

//================================================================//
// TreeSummary
//================================================================//
    
//----------------------------------------------------------------//
void TreeSummary::AnalyzeLevels ( map < size_t, TreeLevelStats >& levels, size_t depth ) const {

    TreeLevelStats& stats = levels [ depth ];
    stats.mChains += this->mChains;
    stats.mContribution += this->mContribution;
    
    list < TreeSummary >::const_iterator childrenIt = this->mChildren.begin ();
    for ( ; childrenIt != this->mChildren.end (); ++ childrenIt ) {
        childrenIt->AnalyzeLevels ( levels, depth + 1 );
    }
}

//----------------------------------------------------------------//
void TreeSummary::ComputePercents ( size_t totalBlocks ) {

    this->mPercentOfTotal = totalBlocks > 0 ? (( float )this->mContribution / ( float )totalBlocks ) : 0.0;
    
    list < TreeSummary >::iterator childrenIt = this->mChildren.begin ();
    for ( ; childrenIt != this->mChildren.end (); ++ childrenIt ) {
        childrenIt->ComputePercents ( totalBlocks );
    }
}

//----------------------------------------------------------------//
size_t TreeSummary::ComputeSize () {
    
    this->mChains = 0;
    this->mSubtreeSize = 0;
    
    if ( this->mChildren.size ()) {
        list < TreeSummary >::iterator childrenIt = this->mChildren.begin ();
        for ( ; childrenIt != this->mChildren.end (); ++ childrenIt ) {
            TreeSummary& child = *childrenIt;
            child.ComputeSize ();
            this->mSubtreeSize += child.mSubtreeSize;
            this->mChains += child.mChains;
        }
        this->mContribution = this->mPlayers.size () * this->mChains;
        this->mSubtreeSize += this->mContribution;
    }
    else {
        this->mChains = 1;
        this->mSubtreeSize = this->mPlayers.size ();
        this->mContribution = this->mSubtreeSize;
    }
    return this->mSubtreeSize;
}

//----------------------------------------------------------------//
size_t TreeSummary::MeasureChain ( float threshold ) const {

    assert ( threshold > 0.5 );

    size_t size = 0;

    const TreeSummary* cursor = this;
    while ( cursor ) {
    
        size += cursor->mPlayers.size ();
        
        const TreeSummary* bestChain = 0;
        list < TreeSummary >::const_iterator childrenIt = cursor->mChildren.begin ();
        for ( ; childrenIt != cursor->mChildren.end (); ++ childrenIt ) {
            const TreeSummary& child = *childrenIt;
            if ( !bestChain || ( bestChain->mChains < child.mChains )) {
                bestChain = &child;
            }
        }
        
        float ratio = bestChain ? ( float )bestChain->mChains / ( float )this->mChains : 0.0;
        if ( ratio < threshold ) break;
        
        cursor = bestChain;
    }

    return size;
}

//----------------------------------------------------------------//
void TreeSummary::Print ( bool verbose, int maxDepth, int depth ) const {

    if (( maxDepth > 0 ) && ( depth >= maxDepth )) return;

    size_t nPlayers = this->mPlayers.size ();

    print_indent ( depth );
    printf ( "[size: %d, branches: %d, percent: %g]", ( int )nPlayers, ( int )this->mChains, this->mPercentOfTotal );
    
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
void TreeSummary::PrintLevels () const {

    size_t totalBlocks = this->mSubtreeSize;

    map < size_t, TreeLevelStats > levels;
    this->AnalyzeLevels ( levels );
    
    size_t maxDepth = levels.size ();
    for ( size_t i = 0; i < maxDepth; ++i ) {
        TreeLevelStats& stats = levels [ i ];
        float percent = ( totalBlocks > 0 ) ? (( float )stats.mContribution / ( float )totalBlocks ) : 0.0;
        //printf ( "[branches: %d, percent: %g]", ( int )stats.mChains, percent );
        printf ( "[%.2f]", percent );
    }
    printf ( "\n" );
}

//----------------------------------------------------------------//
void TreeSummary::Summarize ( const Tree& tree ) {

    this->SummarizeRecurse ( tree );
    
    size_t totalBlocks = this->ComputeSize ();
    this->ComputePercents ( totalBlocks );
}

//----------------------------------------------------------------//
void TreeSummary::SummarizeRecurse ( const Tree& tree ) {

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

//----------------------------------------------------------------//
TreeSummary::TreeSummary () :
    mChains ( 0 ),
    mContribution ( 0 ),
    mSubtreeSize ( 0 ),
    mPercentOfTotal ( 0.0 ) {
}

//================================================================//
// Analysis
//================================================================//
    
//----------------------------------------------------------------//
Analysis::Analysis () :
    mPasses ( 0 ),
    mChainLength ( 0 ),
    mAverageIncrease ( 0.0 ) {
}

//----------------------------------------------------------------//
void Analysis::Print ( bool verbose, int maxDepth ) {

    printf ( "PASS: %d, AVG: %g LEN: %d\n", ( int )this->mPasses, this->mAverageIncrease, ( int )this->mChainLength );
    this->mSummary.PrintLevels ();
    this->mSummary.Print ( verbose, maxDepth );
}

//----------------------------------------------------------------//
void Analysis::Update () {

    this->mSummary = TreeSummary ();
    Context::Summarize ( this->mSummary );
    
    this->mChainLength = this->mSummary.MeasureChain ( 0.65 );
    this->mPassesToLength [ this->mPasses ] = this->mChainLength;
    
    if ( this->mPasses > 0 ) {
        
        int increase = this->mChainLength - ( float )this->mPassesToLength [ this->mPasses - 1 ];
        size_t nextPasses = this->mPasses + 1;
        this->mAverageIncrease = ( this->mAverageIncrease * (( float )this->mPasses / ( float )nextPasses )) + (( float )increase / ( float )nextPasses );
    }
    
    this->mPasses++;
}
