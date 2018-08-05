// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_CHAIN_H
#define VOLITION_CHAIN_H

#include <volition/ChainPlacement.h>
#include <volition/Cycle.h>

namespace Volition {

class State;
class ChainPlacement;

//================================================================//
// Chain
//================================================================//
class Chain :
    public AbstractSerializable {
private:

    State                                                   mState;
    SerializableVector < SerializableUniquePtr < Cycle >>   mCycles;

    //----------------------------------------------------------------//
    bool                    canEdit             ( size_t cycleID, string minerID = "" ) const;
    bool                    canEdit             ( size_t cycleID, const Chain& chain ) const;
    static const Chain*     choose              ( size_t cycleID, const Chain& prefer, const Chain& other );
    size_t                  findMax             ( size_t cycleID ) const;
    Cycle&                  getCycle            ( size_t idx );
    Cycle*                  getTopCycle         ();
    const Cycle*            getTopCycle         () const;
    void                    rebuildState        ();

    //----------------------------------------------------------------//
    void                    AbstractSerializable_serialize      ( AbstractSerializer& serializer ) override;

public:

    //----------------------------------------------------------------//
                            Chain               ();
                            Chain               ( shared_ptr < Block > block );
                            Chain               ( const Chain& chain );
                            ~Chain              ();
    static const Chain*     choose              ( const Chain& chain0, const Chain& chain1 );
    size_t                  countBlocks         ( size_t cycleIdx );
    size_t                  countCycles         () const;
    Block*                  findBlock           ( u64 height );
    ChainPlacement          findPlacement       ( string minerID, bool force ) const;
    Block&                  getBlock            ( size_t cycleIdx, size_t blockIdx );
    const State&            getState            () const;
    void                    getStateSnapshot    ( State& state );
    bool                    pushAndSign         ( const ChainPlacement& placement, shared_ptr < Block > block, const CryptoKey& key, string hashAlgorithm = Signature::DEFAULT_HASH_ALGORITHM );
    void                    print               ( const char* pre = 0, const char* post = "\n" ) const;
};

} // namespace Volition
#endif
