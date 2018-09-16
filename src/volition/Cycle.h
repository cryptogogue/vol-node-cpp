// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_CYCLE_H
#define VOLITION_CYCLE_H

#include <volition/Block.h>

namespace Volition {

class State;

//================================================================//
// Cycle
//================================================================//
class Cycle :
    public AbstractSerializable {
private:

    friend class Chain;
    friend class ChainPlacement;

    u64     mCycleID;
    u64     mBase; // verion where cycle starts

    //SerializableSet < string >                                  mMiners;
    //SerializableVector < SerializableSharedPtr < Block >>       mBlocks;

    //----------------------------------------------------------------//
    bool                    apply                   ( State& state );
    static int              compare                 ( const Cycle& cycle0, const Cycle& cycle1 );
    bool                    containsMiner           ( string minerID ) const;
    size_t                  countBlocks             () const;
    size_t                  countMiners             ( string minerID = "" ) const;
    Block*                  findBlock               ( u64 height );
    u64                     findPosition            ( size_t score ) const;
//    Block&                  getBlock                ( size_t idx );
    size_t                  getID                   () const;
    size_t                  getLength               () const;
    bool                    isInChain               ( string minerID ) const;
    void                    print                   () const;
    void                    rebuildMinerSet         ();
    void                    setID                   ( u64 cycleID );
    bool                    willImprove             ( string minerID ) const;
    

    //----------------------------------------------------------------//
    void                    AbstractSerializable_serialize      ( AbstractSerializer& serializer ) override;

public:
    
    //----------------------------------------------------------------//
                            Cycle                   ();
                            ~Cycle                  ();
};

} // namespace Volition
#endif
