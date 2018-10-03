// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_CHAIN_H
#define VOLITION_CHAIN_H

#include <volition/common.h>
#include <volition/serialization/AbstractSerializable.h>
#include <volition/State.h>

namespace Volition {

class Block;
class Cycle;
class ChainMetadata;
class ChainPlacement;

//================================================================//
// Cycle
//================================================================//
class Cycle {
private:

    friend class Chain;
    friend class ChainPlacement;

    u64     mCycleID;
    u64     mBase; // verion where cycle starts

public:
    
    //----------------------------------------------------------------//
    Cycle () :
        mCycleID ( 0 ),
        mBase ( 0 ) {
    }

    //----------------------------------------------------------------//
    Cycle ( size_t cycleID, size_t base ) :
        mCycleID ( cycleID ),
        mBase ( base ) {
    }
};

//================================================================//
// ChainPlacement
//================================================================//
class ChainPlacement {
private:

    friend class Chain;
    friend class Cycle;

    bool            mNewCycle;
    Cycle           mCycle;

    //----------------------------------------------------------------//
    ChainPlacement ( const Cycle& cycle, bool newCycle ) :
        mNewCycle ( newCycle ),
        mCycle ( cycle ) {
    }

public:

    //----------------------------------------------------------------//
    size_t getCycleID () const {

        return this->mCycle.mCycleID + ( this->mNewCycle ? 1 : 0 );
    }
};

//================================================================//
// Chain
//================================================================//
class Chain :
    public AbstractSerializable,
    public State {
private:

    //----------------------------------------------------------------//
    Cycle                       getTopCycle         () const;
    bool                        isInCycle           ( const Cycle& cycle, string minerID );
    void                        newCycle            ();
    bool                        willImprove         ( ChainMetadata& metaData, const Cycle& cycle, string minerID );

    //----------------------------------------------------------------//
    void                        AbstractSerializable_serialize      ( AbstractSerializer& serializer ) override;

public:

    static constexpr const char* BLOCK_KEY      = "block";
    static constexpr const char* CYCLE_KEY      = "cycle";

    //----------------------------------------------------------------//
    bool                        canPush             ( string minerID, bool force );
                                Chain               ();
                                ~Chain              ();
    size_t                      countBlocks         ();
    size_t                      countBlocks         ( size_t cycleIdx );
    size_t                      countCycles         () const;
    ChainPlacement              findNextCycle       ( ChainMetadata& metaData, string minerID );
    VersionedValue < Block >    getBlock            ( size_t cycleIdx, size_t blockIdx ) const;
    VersionedValue < Block >    getTopBlock         () const;
    void                        prepareForPush      ( ChainMetadata& metaData, const ChainPlacement& placement, Block& block );
    string                      print               ( const ChainMetadata& metaData, const char* pre = 0, const char* post = 0 );
    bool                        pushBlock           ( Block& block );
    void                        reset               ();
    size_t                      size                () const;
    void                        update              ( ChainMetadata& metaData, Chain& other );
};

} // namespace Volition
#endif
