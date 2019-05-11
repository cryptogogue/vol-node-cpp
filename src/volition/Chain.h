// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_CHAIN_H
#define VOLITION_CHAIN_H

#include <volition/common.h>
#include <volition/serialization/AbstractSerializable.h>
#include <volition/Ledger.h>

namespace Volition {

class Block;
class ChainMetadata;

//================================================================//
// Chain
//================================================================//
class Chain :
    public AbstractSerializable,
    public Ledger {
private:

    //----------------------------------------------------------------//
    static int                  compareSegment      ( const Chain& chain0, const Chain& chain1, size_t base, size_t n );

    //----------------------------------------------------------------//
    void                        AbstractSerializable_serializeFrom      ( const AbstractSerializerFrom& serializer ) override;
    void                        AbstractSerializable_serializeTo        ( AbstractSerializerTo& serializer ) const override;

public:

    enum class UpdateResult {
        UPDATE_ACCEPTED,
        UPDATE_EQUALS,
        UPDATE_RETRY,
        UPDATE_REWIND,
    };

    //----------------------------------------------------------------//
    bool                checkMiners         ( string miners ) const;
    static int          compare             ( const Chain& chain0, const Chain& chain1, u64 now, u64 window );
                        Chain               ();
                        ~Chain              ();
    size_t              countBlocks         () const;
    size_t              countBlocks         ( size_t cycleIdx ) const;
    string              print               ( const char* pre = NULL, const char* post = NULL ) const;
    bool                pushBlock           ( const Block& block );
    void                reset               ();
    size_t              size                () const;
};

} // namespace Volition
#endif
