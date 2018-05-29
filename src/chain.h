#ifndef CHAIN_H
#define CHAIN_H

#include "common.h"

//================================================================//
// Block
//================================================================//
//class Block {
//private:
//
//  friend class Chain;
//  friend class Cycle;
//  friend class Tree;
//
//  int         mPlayerID;
//  uint        mScore;
//  Block*      mParent;
//
//public:
//
//  //----------------------------------------------------------------//
//                  Block           ();
//};

//================================================================//
// Cycle
//================================================================//
class Cycle {
private:

    friend class Chain;
    friend class Tree;

    set < int >     mPlayers;
    vector < int >  mChain;

    int             mCycleID;
    int             mEntropy;

    float           mNewPlayerRatio;

    //----------------------------------------------------------------//
    int             FindPosition        ( int playerID ) const;
    bool            IsInChain           ( int playerID ) const;

public:

    //----------------------------------------------------------------//
    static int              Compare                 ( const Cycle& cycle0, const Cycle& cycle1 );
    bool                    Contains                ( int playerID ) const;
    void                    CopyChain               ( const Cycle& cycle );
    size_t                  CountParticipants       ( int playerID = -1 ) const;
                            Cycle                   ();
    size_t                  GetLength               ();
    bool                    Improve                 ( int playerID );
    void                    MergeParticipants       ( const Cycle& cycle );
    void                    Print                   () const;
    void                    SetID                   ( int cycleID );
    void                    UpdatePlayerRatio       ( size_t prevCount );
};

//================================================================//
// Chain
//================================================================//
class Chain {
private:

    friend class Tree;

    vector < Cycle >        mCycles;

    //----------------------------------------------------------------//
    bool                    CanEdit             ( size_t cycleID, int playerID = -1 ) const;
    bool                    CanEdit             ( size_t cycleID, const Chain& chain ) const;
    static const Chain&     Choose              ( size_t cycleID, const Chain& prefer, const Chain& other );
    size_t                  FindMax             ( size_t cycleID ) const;
    Cycle*                  GetTopCycle         ();
    const Cycle*            GetTopCycle         () const;

public:

    //----------------------------------------------------------------//
                            Chain               ();
    static const Chain&     Choose              ( const Chain& chain0, const Chain& chain1 );
    int                     FindPosition        ( int playerID ) const;
    //void                  Knit                ( const Chain& chain );
    void                    MergeParticipants   ( const Chain& chain );
    void                    Print               ( const char* pre = 0, const char* post = "\n" ) const;
    void                    Push                ( int playerID );
    void                    Push                ( int playerID, bool force );
};

#endif

