// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SIMULATOR_ANALYSIS_H
#define VOLITION_SIMULATOR_ANALYSIS_H

#include <volition/common.h>
#include <volition/Chain.h>

namespace Volition {
namespace Simulator {

//================================================================//
// Tree
//================================================================//
class Tree {
private:

    friend class TreeSummary;

    string                  mMinerID;
    map < string, Tree >    mChildren;

public:

    //----------------------------------------------------------------//
    void        addChain        ( const Chain& chain );
                Tree            ();
};

//================================================================//
// TreeLevelStats
//================================================================//
class TreeLevelStats {
public:

    size_t      mChains;
    size_t      mContribution;
    
    //----------------------------------------------------------------//
                TreeLevelStats      ();
};

//================================================================//
// TreeSummary
//================================================================//
class TreeSummary {
private:

    vector < string >       mMiners;
    list < TreeSummary >    mChildren;
    
    size_t                  mChains;
    size_t                  mContribution;
    size_t                  mSubtreeSize;
    float                   mPercentOfTotal;

    //----------------------------------------------------------------//
    void        computePercents     ( size_t totalBlocks );
    size_t      computeSize         ();
    void        summarizeRecurse    ( const Tree& tree );

public:
    
    //----------------------------------------------------------------//
    void        analyzeLevels       ( map < size_t, TreeLevelStats >& levels, size_t depth = 0 ) const;
    void        log                 ( string prefix, bool verbose, int maxDepth = 0, int depth = 0 ) const;
    void        logLevels           ( string prefix ) const;
    size_t      measureChain        ( float threshold ) const;
    void        summarize           ( const Tree& tree );
                TreeSummary         ();
};

//================================================================//
// Analysis
//================================================================//
class Analysis {
private:

    friend class Context;

    map < size_t, size_t >  mPassesToLength;

    size_t                  mPasses;
    TreeSummary             mSummary;
    
    size_t                  mChainLength;
    float                   mAverageIncrease;

public:

    //----------------------------------------------------------------//
                Analysis            ();
    void        log                 ( string prefix, bool verbose = false, int maxDepth = 0 );
    void        update              ();
};

} // namespace Simulator
} // namespace Volition
#endif
