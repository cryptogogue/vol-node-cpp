// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_CYCLE_H
#define VOLITION_CYCLE_H

#include <Block.h>

namespace Volition {

class State;

//================================================================//
// Cycle
//================================================================//
class Cycle :
    public AbstractSerializable {
private:

    size_t                                  mCycleID;

    set < string >                          mMiners;
    vector < unique_ptr < const Block >>    mBlocks;

    //----------------------------------------------------------------//
    int                     findPosition            ( size_t score ) const;

    //----------------------------------------------------------------//
    void                    AbstractSerializable_fromJSON       ( const Poco::JSON::Object& object ) override;
    void                    AbstractSerializable_toJSON         ( Poco::JSON::Object& object ) const override;

public:

    //----------------------------------------------------------------//
    void                    apply                   ( State& state ) const;
    static int              compare                 ( const Cycle& cycle0, const Cycle& cycle1 );
    bool                    containsMiner           ( string minerID ) const;
    size_t                  countMiners             ( string minerID = "" ) const;
                            Cycle                   ();
                            ~Cycle                  ();
    size_t                  getID                   () const;
    size_t                  getLength               () const;
    bool                    isInChain               ( string minerID ) const;
    void                    push                    ( unique_ptr < const Block > block );
    void                    print                   () const;
    void                    setID                   ( size_t cycleID );
    bool                    verify                  ( const State& state ) const;
    bool                    willImprove             ( string minerID );
};

} // namespace Volition
#endif
