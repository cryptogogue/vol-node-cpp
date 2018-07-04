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

    u64                                     mCycleID;

    set < string >                          mMiners;
    vector < shared_ptr < const Block >>    mBlocks;

    //----------------------------------------------------------------//
    void                    apply                   ( State& state ) const;
    static int              compare                 ( const Cycle& cycle0, const Cycle& cycle1 );
    bool                    containsMiner           ( string minerID ) const;
    size_t                  countBlocks             () const;
    size_t                  countMiners             ( string minerID = "" ) const;
    u64                     findPosition            ( size_t score ) const;
    const Block&            getBlock                ( size_t idx ) const;
    size_t                  getID                   () const;
    size_t                  getLength               () const;
    bool                    isInChain               ( string minerID ) const;
    void                    print                   () const;
    void                    setID                   ( u64 cycleID );
    bool                    verify                  ( const State& state ) const;
    bool                    willImprove             ( string minerID ) const;
    

    //----------------------------------------------------------------//
    void                    AbstractSerializable_fromJSON       ( const Poco::JSON::Object& object ) override;
    void                    AbstractSerializable_toJSON         ( Poco::JSON::Object& object ) const override;

public:
    
    //----------------------------------------------------------------//
                            Cycle                   ();
                            ~Cycle                  ();
};

} // namespace Volition
#endif
