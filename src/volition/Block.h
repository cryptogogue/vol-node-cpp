// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_BLOCK_H
#define VOLITION_BLOCK_H

#include <volition/common.h>
#include <volition/BlockHeader.h>
#include <volition/LedgerResult.h>

namespace Volition {

class Ledger;
class Transaction;

//================================================================//
// BlockBody
//================================================================//
class BlockBody :
    public virtual AbstractSerializable {
private:

    friend class Context;
    friend class AbstractLedger;
    friend class Block;

    string              mReward;

    // TODO: store these in a map indexed by maturity (so we don't have to traverse all transactions when handling deferred transactions)
    SerializableVector < SerializableSharedConstPtr < Transaction >> mTransactions;
    
    //----------------------------------------------------------------//
    void        AbstractSerializable_serializeFrom      ( const AbstractSerializerFrom& serializer ) override;
    void        AbstractSerializable_serializeTo        ( AbstractSerializerTo& serializer ) const override;

public:

    //----------------------------------------------------------------//
                BlockBody           ();
                ~BlockBody          ();
};

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
    friend class AbstractLedger;

    u64                             mBodyType;
    mutable string                  mBodyString;
    shared_ptr < BlockBody >        mBody;

    //----------------------------------------------------------------//
    void                affirmBody                          ();
    LedgerResult        applyTransactions                   ( Ledger& ledger, VerificationPolicy policy, size_t& nextMaturity ) const;
    size_t              getWeight                           () const;
    
    //----------------------------------------------------------------//
    void                AbstractSerializable_serializeFrom      ( const AbstractSerializerFrom& serializer ) override;
    void                AbstractSerializable_serializeTo        ( AbstractSerializerTo& serializer ) const override;

public:

    //----------------------------------------------------------------//
    void                affirmHash                          ();
    LedgerResult        apply                               ( Ledger& ledger, VerificationPolicy policy ) const;
                        Block                               ();
                        Block                               ( string bodyString );
                        ~Block                              ();
    size_t              countTransactions                   () const;
    void                pushTransaction                     ( shared_ptr < const Transaction > transaction );
    const Digest&       sign                                ( const CryptoKeyPair& key, string hashAlgorithm = Digest::DEFAULT_HASH_ALGORITHM );
    void                setReward                           ( string reward );
    LedgerResult        verify                              ( const Ledger& ledger, VerificationPolicy policy ) const;
};

} // namespace Volition
#endif
