// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_CHAIN_H
#define VOLITION_CHAIN_H

#include <volition/common.h>
#include <volition/serialization/AbstractSerializable.h>
#include <volition/ChainPlacement.h>
#include <volition/State.h>

namespace Volition {

class Block;
class Cycle;
class ChainMetadata;
class ChainPlacement;

//================================================================//
// Chain
//================================================================//
class Chain :
    public AbstractSerializable,
    public State {
private:

    //----------------------------------------------------------------//
    size_t                      findMax             ( size_t cycleID ) const;
    //const Cycle*              getCycle            ( size_t idx );
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
    static const Chain*         choose              ( const Chain& chain0, const Chain& chain1 );
    size_t                      countBlocks         ();
    size_t                      countBlocks         ( size_t cycleIdx );
    size_t                      countCycles         () const;
    Block*                      findBlock           ( u64 height );
    ChainPlacement              findNextCycle       ( ChainMetadata& metaData, string minerID );
    VersionedValue < Block >    getBlock            ( size_t cycleIdx, size_t blockIdx ) const;
    VersionedValue < Block >    getTopBlock         () const;
    void                        prepareForPush      ( ChainMetadata& metaData, const ChainPlacement& placement, Block& block );
    string                      print               ( const ChainMetadata& metaData, const char* pre = 0, const char* post = 0 );
    bool                        pushBlock           ( Block& block );
    void                        reset               ();
    size_t                      size                () const;
    size_t                      truncate            ( const ChainPlacement& placement, size_t score );
    void                        update              ( ChainMetadata& metaData, Chain& other );
};

} // namespace Volition
#endif
