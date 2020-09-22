// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SIMULATION_ANALYSIS_H
#define VOLITION_SIMULATION_ANALYSIS_H

#include <volition/common.h>
#include <volition/Chain.h>

namespace Volition {
namespace Simulation {

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
protected:

    vector < string >       mMiners;
    list < TreeSummary >    mChildren;
    
    size_t                  mChains;
    size_t                  mContribution;
    size_t                  mSubtreeSize;
    float                   mPercentOfTotal;

    //----------------------------------------------------------------//
    void        analyzeLevels       ( map < size_t, TreeLevelStats >& levels, size_t depth = 0 ) const;
    void        computePercents     ( size_t totalBlocks );
    size_t      computeSize         ();
    void        summarizeRecurse    ( const Tree& tree );

public:
    
    //----------------------------------------------------------------//
    void        clear               ();
    void        logTree             ( string prefix, bool verbose, size_t maxDepth = 0, size_t depth = 0 ) const;
    size_t      measureChain        ( double threshold ) const;
    void        summarize           ( const Tree& tree );
                TreeSummary         ();
};

//================================================================//
// Analysis
//================================================================//
class Analysis :
    public TreeSummary {
private:

    map < size_t, size_t >          mPassesToLength;
    map < size_t, TreeLevelStats >  mLevels;

    size_t                  mPasses;
    
    size_t                  mChainLength;
    double                  mAverageIncrease;

public:

    //----------------------------------------------------------------//
                Analysis            ();
    void        clear               ();
    size_t      countLevels         () const;
    float       getLevelPercent     ( size_t level ) const;
    void        log                 ( string prefix, bool verbose = false, size_t maxDepth = 0 ) const;
    void        logLevels           ( string prefix ) const;
    void        update              ( const Tree& tree );
};

} // namespace Simulation
} // namespace Volition
#endif
