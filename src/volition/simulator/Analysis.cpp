// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/simulator/Analysis.h>
#include <volition/Block.h>
#include <volition/simulator/SimMiner.h>
#include <volition/simulator/TheSimulator.h>

namespace Volition {
namespace Simulator {

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
void Tree::addChain ( Chain& chain ) {

    Tree* cursor = this;

    VersionedStoreIterator chainIt ( chain, 0 );
    
    size_t top = chain.getVersion ();
    for ( ; chainIt && ( chainIt.getVersion () < top ); chainIt.next ()) {
        const Block& block = chainIt.getValue < Block >( Chain::BLOCK_KEY );
        
        string minerID      = block.getMinerID ();
        cursor              = &cursor->mChildren [ minerID ];
        cursor->mMinerID    = minerID;
    }
}

//----------------------------------------------------------------//
Tree::Tree () {
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
void TreeSummary::analyzeLevels ( map < size_t, TreeLevelStats >& levels, size_t depth ) const {

    TreeLevelStats& stats = levels [ depth ];
    stats.mChains += this->mChains;
    stats.mContribution += this->mContribution;
    
    list < TreeSummary >::const_iterator childrenIt = this->mChildren.begin ();
    for ( ; childrenIt != this->mChildren.end (); ++ childrenIt ) {
        childrenIt->analyzeLevels ( levels, depth + 1 );
    }
}

//----------------------------------------------------------------//
void TreeSummary::computePercents ( size_t totalBlocks ) {

    this->mPercentOfTotal = totalBlocks > 0 ? (( float )this->mContribution / ( float )totalBlocks ) : 0.0;
    
    list < TreeSummary >::iterator childrenIt = this->mChildren.begin ();
    for ( ; childrenIt != this->mChildren.end (); ++ childrenIt ) {
        childrenIt->computePercents ( totalBlocks );
    }
}

//----------------------------------------------------------------//
size_t TreeSummary::computeSize () {
    
    this->mChains = 0;
    this->mSubtreeSize = 0;
    
    if ( this->mChildren.size ()) {
        list < TreeSummary >::iterator childrenIt = this->mChildren.begin ();
        for ( ; childrenIt != this->mChildren.end (); ++ childrenIt ) {
            TreeSummary& child = *childrenIt;
            child.computeSize ();
            this->mSubtreeSize += child.mSubtreeSize;
            this->mChains += child.mChains;
        }
        this->mContribution = this->mMiners.size () * this->mChains;
        this->mSubtreeSize += this->mContribution;
    }
    else {
        this->mChains = 1;
        this->mSubtreeSize = this->mMiners.size ();
        this->mContribution = this->mSubtreeSize;
    }
    return this->mSubtreeSize;
}

//----------------------------------------------------------------//
size_t TreeSummary::measureChain ( float threshold ) const {

    assert ( threshold > 0.5 );

    size_t size = 0;

    const TreeSummary* cursor = this;
    while ( cursor ) {
    
        size += cursor->mMiners.size ();
        
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
void TreeSummary::print ( bool verbose, int maxDepth, int depth ) const {

    if (( maxDepth > 0 ) && ( depth >= maxDepth )) return;

    size_t nMiners = this->mMiners.size ();

    print_indent ( depth );
    printf ( "[size: %d, branches: %d, percent: %g]", ( int )nMiners, ( int )this->mChains, this->mPercentOfTotal );
    
    if ( verbose && ( nMiners > 0 )) {
        printf ( " - " );
        for ( size_t i = 0; i < nMiners; ++i ) {
            if ( i > 0 ) {
                printf ( "," );
            }
            printf ( "%s", this->mMiners [ i ].c_str ());
        }
    }
    printf ( "\n" );
    
    ++depth;
    list < TreeSummary >::const_iterator childrenIt = this->mChildren.begin ();
    for ( ; childrenIt != this->mChildren.end (); ++ childrenIt ) {
        childrenIt->print ( verbose, maxDepth, depth );
    }
}

//----------------------------------------------------------------//
void TreeSummary::printLevels () const {

    size_t totalBlocks = this->mSubtreeSize;

    map < size_t, TreeLevelStats > levels;
    this->analyzeLevels ( levels );
    
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
void TreeSummary::summarize ( const Tree& tree ) {

    this->summarizeRecurse ( tree );
    
    size_t totalBlocks = this->computeSize ();
    this->computePercents ( totalBlocks );
}

//----------------------------------------------------------------//
void TreeSummary::summarizeRecurse ( const Tree& tree ) {

    size_t nChildren = tree.mChildren.size ();

    if ( nChildren ) {

        map < string, Tree >::const_iterator childrenIt = tree.mChildren.begin ();
        
        if ( nChildren == 1 ) {
            this->mMiners.push_back ( childrenIt->second.mMinerID );
            this->summarizeRecurse ( childrenIt->second );
        }
        else {

            for ( ; childrenIt != tree.mChildren.end (); ++childrenIt ) {
                this->mChildren.push_back ( TreeSummary ());
                TreeSummary& childSummary = this->mChildren.back ();
                childSummary.mMiners.push_back ( childrenIt->second.mMinerID );
                childSummary.summarizeRecurse ( childrenIt->second );
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
void Analysis::print ( bool verbose, int maxDepth ) {

    printf ( "PASS: %d, AVG: %g LEN: %d\n", ( int )this->mPasses, this->mAverageIncrease, ( int )this->mChainLength );
    this->mSummary.printLevels ();
    this->mSummary.print ( verbose, maxDepth );
}

//----------------------------------------------------------------//
void Analysis::update () {

    this->mSummary = TreeSummary ();
    TheSimulator::get ().summarize ( this->mSummary );
    
    this->mChainLength = this->mSummary.measureChain ( 0.65 );
    this->mPassesToLength [ this->mPasses ] = this->mChainLength;
    
    if ( this->mPasses > 0 ) {
        
        int increase = this->mChainLength - ( float )this->mPassesToLength [ this->mPasses - 1 ];
        size_t nextPasses = this->mPasses + 1;
        this->mAverageIncrease = ( this->mAverageIncrease * (( float )this->mPasses / ( float )nextPasses )) + (( float )increase / ( float )nextPasses );
    }
    
    this->mPasses++;
}

} // namespace Simulator
} // namespace Volition
