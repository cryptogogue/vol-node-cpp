// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/simulation/Analysis.h>
#include <volition/Block.h>
#include <volition/Format.h>

namespace Volition {
namespace Simulation {

//----------------------------------------------------------------//
void print_indent ( string& str, int indent ) {

    for ( int i = 0; i < indent; ++i ) {
        Format::write ( str, ".   " );
    }
}

//================================================================//
// Tree
//================================================================//

//----------------------------------------------------------------//
void Tree::addChain ( const Chain& chain ) {

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
void TreeSummary::clear () {

    this->mMiners.clear ();
    this->mChildren.clear ();
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
void TreeSummary::logTree ( string prefix, bool verbose, int maxDepth, int depth ) const {

    if (( maxDepth > 0 ) && ( depth >= maxDepth )) return;

    size_t nMiners = this->mMiners.size ();

    string str;
    print_indent ( str, depth );
    Format::write ( str, "[size: %d, branches: %d, percent: %g]", ( int )nMiners, ( int )this->mChains, this->mPercentOfTotal );
    
    if ( verbose && ( nMiners > 0 )) {
        Format::write ( str, " - " );
        for ( size_t i = 0; i < nMiners; ++i ) {
            if ( i > 0 ) {
                Format::write ( str, "," );
            }
            Format::write ( str, "%s", this->mMiners [ i ].c_str ());
        }
    }
    LOG_F ( INFO, "%s%s", prefix.c_str (), str.c_str ());
    
    ++depth;
    list < TreeSummary >::const_iterator childrenIt = this->mChildren.begin ();
    for ( ; childrenIt != this->mChildren.end (); ++ childrenIt ) {
        childrenIt->logTree ( prefix, verbose, maxDepth, depth );
    }
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
void Analysis::clear () {

    this->TreeSummary::clear ();
    this->mPassesToLength.clear ();
    this->mLevels.clear ();
}

//----------------------------------------------------------------//
size_t Analysis::countLevels () const {

    return this->mLevels.size ();
}

//----------------------------------------------------------------//
float Analysis::getLevelPercent ( size_t level ) const {

    size_t totalBlocks = this->mSubtreeSize;

     map < size_t, TreeLevelStats >::const_iterator levelIt = this->mLevels.find ( level );
    if ( levelIt != this->mLevels.cend ()) {
        const TreeLevelStats& stats = levelIt->second;
        return ( totalBlocks > 0 ) ? (( float )stats.mContribution / ( float )totalBlocks ) : 0.0;
    }
    return 0.0;
}

//----------------------------------------------------------------//
void Analysis::log ( string prefix, bool verbose, int maxDepth ) const {

    LOG_F ( INFO, "%sPASS: %d, AVG: %g LEN: %d\n", prefix.c_str (), ( int )this->mPasses, this->mAverageIncrease, ( int )this->mChainLength );
    this->logLevels ( prefix );
    this->logTree ( prefix, verbose, maxDepth );
}

//----------------------------------------------------------------//
void Analysis::logLevels ( string prefix ) const {

    string str;
    size_t maxDepth = this->mLevels.size ();
    for ( size_t i = 0; i < maxDepth; ++i ) {
        float percent = this->getLevelPercent ( i );
        //Format::write ( str, "[branches: %d, percent: %g]", ( int )stats.mChains, percent );
        Format::write ( str, "[%.2f]", percent );
    }
    LOG_F ( INFO, "%s%s", prefix.c_str (), str.c_str ());
}

//----------------------------------------------------------------//
void Analysis::update ( const Tree& tree ) {

    this->clear ();
    this->summarize ( tree );
    
    this->mChainLength = this->measureChain ( 0.65 );
    this->mPassesToLength [ this->mPasses ] = this->mChainLength;
    this->analyzeLevels ( this->mLevels );
    
    if ( this->mPasses > 0 ) {
        
        int increase = this->mChainLength - ( float )this->mPassesToLength [ this->mPasses - 1 ];
        size_t nextPasses = this->mPasses + 1;
        this->mAverageIncrease = ( this->mAverageIncrease * (( float )this->mPasses / ( float )nextPasses )) + (( float )increase / ( float )nextPasses );
    }
    
    this->mPasses++;
}

} // namespace Simulator
} // namespace Volition
