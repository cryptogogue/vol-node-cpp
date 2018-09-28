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

    static constexpr const char* BLOCK_KEY      = "block";
    static constexpr const char* CYCLE_KEY      = "cycle";

    shared_ptr < ChainMetadata >                mMetaData;

    //----------------------------------------------------------------//
    bool                    canEdit             ( const Cycle& cycle0, const Cycle& cycle1, string minerID = "" );
    bool                    canEdit             ( size_t cycleID, const Chain& chain ) const;
    static const Chain*     choose              ( size_t cycleID, const Chain& prefer, const Chain& other );
    size_t                  countParticipants   ( const Cycle& cycle, string minerID = "" );
    size_t                  findMax             ( size_t cycleID ) const;
    //const Cycle*            getCycle            ( size_t idx );
    Cycle                   getTopCycle         () const;
    bool                    isInCycle           ( const Cycle& cycle, string minerID );
    bool                    isParticipant       ( const Cycle& cycle, string minerID ) const;
    void                    newCycle            ();
    void                    rebuildState        ();
    bool                    willImprove         ( const Cycle& cycle, string minerID );

    //----------------------------------------------------------------//
    void                    AbstractSerializable_serialize      ( AbstractSerializer& serializer ) override;

public:

    //----------------------------------------------------------------//
    bool                        canPush             ( string minerID, bool force );
                                Chain               ();
                                Chain               ( Block& genesisBlock );
                                Chain               ( const Chain& chain );
                                ~Chain              ();
    static const Chain*         choose              ( const Chain& chain0, const Chain& chain1 );
    size_t                      countBlocks         ();
    size_t                      countBlocks         ( size_t cycleIdx );
    size_t                      countCycles         () const;
    Block*                      findBlock           ( u64 height );
    ChainPlacement              findNextCycle       ( string minerID );
    VersionedValue < Block >    getBlock            ( size_t cycleIdx, size_t blockIdx ) const;
    VersionedValue < Block >    getTopBlock         () const;
    void                        prepareForPush      ( const ChainPlacement& placement, Block& block );
    void                        print               ( const char* pre = 0, const char* post = "\n" ) const;
    bool                        pushBlock           ( Block& block );
    bool                        pushBlockAndSign    ( Block& block, const CryptoKey& key, string hashAlgorithm = Signature::DEFAULT_HASH_ALGORITHM );
    void                        reset               ();
    size_t                      size                () const;
    size_t                      truncate            ( const ChainPlacement& placement, size_t score );
};

} // namespace Volition
#endif
