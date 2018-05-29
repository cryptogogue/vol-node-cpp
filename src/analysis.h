#ifndef ANALYSIS_H
#define ANALYSIS_H

#include "common.h"

class Chain;

//================================================================//
// Tree
//================================================================//
class Tree {
private:

    friend class TreeSummary;

    int                 mPlayer;
    map < int, Tree >   mChildren;

public:

    //----------------------------------------------------------------//
    void        AddChain        ( const Chain& chain );
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

    vector < int >          mPlayers;
    list < TreeSummary >    mChildren;
    
    size_t                  mChains;
    size_t                  mContribution;
    size_t                  mSubtreeSize;
    float                   mPercentOfTotal;

    //----------------------------------------------------------------//
    void        ComputePercents     ( size_t totalBlocks );
    size_t      ComputeSize         ();
    void        SummarizeRecurse    ( const Tree& tree );

public:
    
    //----------------------------------------------------------------//
    void        AnalyzeLevels       ( map < size_t, TreeLevelStats >& levels, size_t depth = 0 ) const;
    size_t      MeasureChain        ( float threshold ) const;
    void        Print               ( bool verbose, int maxDepth = 0, int depth = 0 ) const;
    void        PrintLevels         () const;
    void        Summarize           ( const Tree& tree );
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
    void        Print               ( bool verbose = false, int maxDepth = 0 );
    void        Update              ();
};

#endif
