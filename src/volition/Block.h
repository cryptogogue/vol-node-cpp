// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_BLOCK_H
#define VOLITION_BLOCK_H

#include <volition/common.h>
#include <volition/BlockHeader.h>

namespace Volition {

class Ledger;
class Transaction;

//================================================================//
// Block
//================================================================//
class Block :
    public virtual AbstractSerializable,
    public BlockHeader {
public:

    enum VerificationPolicy {
        ALL                         = -1,
        VERIFY_POSE                 = 1 << 0,
        VERIFY_CHARM                = 1 << 1,
        VERIFY_BLOCK_SIG            = 1 << 2,
        VERIFY_TRANSACTION_SIG      = 1 << 3,
        NONE                        = 0,
    };

private:

    friend class Context;
    friend class Ledger;

    string              mReward;

    // TODO: store these in a map indexed by maturity (so we don't have to traverse all transactions when handling deferred transactions)
    SerializableVector < SerializableSharedConstPtr < Transaction >> mTransactions;

    //----------------------------------------------------------------//
    size_t              applyTransactions                   ( Ledger& ledger, VerificationPolicy policy ) const;
    size_t              getWeight                           () const;
    
    //----------------------------------------------------------------//
    void                AbstractSerializable_serializeFrom      ( const AbstractSerializerFrom& serializer ) override;
    void                AbstractSerializable_serializeTo        ( AbstractSerializerTo& serializer ) const override;

public:

    GET_SET ( string, Reward, mReward )

    //----------------------------------------------------------------//
    void                affirmHash                          ();
    bool                apply                               ( Ledger& ledger, VerificationPolicy policy ) const;
                        Block                               ();
                        ~Block                              ();
    size_t              countTransactions                   () const;
    void                pushTransaction                     ( shared_ptr < const Transaction > transaction );
    const Digest&       sign                                ( const CryptoKeyPair& key, string hashAlgorithm = Digest::DEFAULT_HASH_ALGORITHM );
    bool                verify                              ( const Ledger& ledger, VerificationPolicy policy ) const;
};

} // namespace Volition
#endif
