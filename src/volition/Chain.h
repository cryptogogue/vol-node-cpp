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
    bool                        isInCycle           ( const Cycle& cycle, string minerID ) const;
    void                        newCycle            ();
    string                      print               ( const ChainMetadata* metaData, const char* pre, const char* post ) const;
    bool                        willImprove         ( ChainMetadata& metaData, const Cycle& cycle, string minerID ) const;

    //----------------------------------------------------------------//
    void                        AbstractSerializable_serializeFrom      ( const AbstractSerializerFrom& serializer ) override;
    void                        AbstractSerializable_serializeTo        ( AbstractSerializerTo& serializer ) const override;

public:

    static constexpr const char* BLOCK_KEY      = "block";
    static constexpr const char* CYCLE_KEY      = "cycle";

    enum class UpdateResult {
        UPDATE_ACCEPTED,
        UPDATE_EQUALS,
        UPDATE_RETRY,
        UPDATE_REWIND,
    };

    //----------------------------------------------------------------//
    bool                        canPush             ( string minerID, bool force ) const;
                                Chain               ();
                                ~Chain              ();
    size_t                      countBlocks         () const;
    size_t                      countBlocks         ( size_t cycleIdx ) const;
    size_t                      countCycles         () const;
    ChainPlacement              findNextCycle       ( ChainMetadata& metaData, string minerID ) const;
    VersionedValue < Block >    getBlock            ( size_t height ) const;
    VersionedValue < Block >    getTopBlock         () const;
    void                        prepareForPush      ( ChainMetadata& metaData, const ChainPlacement& placement, Block& block );
    string                      print               ( const char* pre = NULL, const char* post = NULL ) const;
    string                      print               ( const ChainMetadata& metaData, const char* pre = NULL, const char* post = NULL ) const;
    bool                        pushBlock           ( const Block& block );
    void                        reset               ();
    size_t                      size                () const;
    UpdateResult                update              ( ChainMetadata& metaData, const Block& block );
    void                        update              ( ChainMetadata& metaData, const Chain& other );
};

} // namespace Volition
#endif
