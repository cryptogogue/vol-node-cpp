// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_BLOCK_H
#define VOLITION_BLOCK_H

#include <volition/common.h>

#include <volition/BlockHeader.h>
#include <volition/Ledger.h>
#include <volition/Transaction.h>

namespace Volition {

//================================================================//
// Block
//================================================================//
class Block :
    public BlockHeader {
private:

    friend class Context;
    friend class Chain;
    friend class Ledger;

    // TODO: store these in a map indexed by maturity (so we don't have to traverse all transactions when handling deferred transactions)
    SerializableVector < SerializableSharedConstPtr < Transaction >> mTransactions;

    //----------------------------------------------------------------//
    size_t              applyTransactions                   ( Ledger& ledger ) const;
    
    //----------------------------------------------------------------//
    void                AbstractSerializable_serializeFrom      ( const AbstractSerializerFrom& serializer ) override;
    void                AbstractSerializable_serializeTo        ( AbstractSerializerTo& serializer ) const override;

public:

    enum VerificationPolicy {
        ALL                 = -1,
        VERIFY_POSE       = 1 << 0,
        VERIFY_CHARM        = 1 << 1,
        VERIFY_SIG          = 1 << 2,
        NONE                = 0,
    };

    //----------------------------------------------------------------//
    void                affirmHash                          ();
    bool                apply                               ( Ledger& ledger, VerificationPolicy policy ) const;
                        Block                               ();
                        Block                               ( string minerID, const Digest& visage, time_t now, const Block* prevBlock, const CryptoKey& key );
                        ~Block                              ();
    size_t              countTransactions                   () const;
    void                pushTransaction                     ( shared_ptr < const Transaction > transaction );
    const Digest&       sign                                ( const CryptoKey& key, string hashAlgorithm = Digest::DEFAULT_HASH_ALGORITHM );
    bool                verify                              ( const Ledger& ledger, VerificationPolicy policy ) const;
};

} // namespace Volition
#endif
