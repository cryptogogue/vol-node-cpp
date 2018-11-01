// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_BLOCK_H
#define VOLITION_BLOCK_H

#include <volition/common.h>

#include <volition/AbstractTransaction.h>
#include <volition/Signature.h>
#include <volition/Ledger.h>
#include <volition/TheTransactionFactory.h>
#include <volition/serialization/Serialization.h>

namespace Volition {

//================================================================//
// Block
//================================================================//
class Block :
    public AbstractSerializable {
private:

    friend class Context;
    friend class Chain;
    friend class Cycle;

    string          mMinerID;
    u64             mCycleID;
    u64             mHeight;
    Digest          mPrevDigest;
    Digest          mAllure; // digital signature of the hash of mCycleID
    Signature       mSignature;

    SerializableVector < SerializableSharedPtr < AbstractTransaction, TransactionFactory >>     mTransactions;

    //----------------------------------------------------------------//
    void                setCycleID                          ( size_t cycleID );
    void                setPreviousBlock                    ( const Block& prevBlock );
    bool                verify                              ( const Ledger& ledger ) const;
    bool                verify                              ( const Ledger& ledger, const CryptoKey& key ) const;

    //----------------------------------------------------------------//
    void                AbstractSerializable_serializeFrom      ( const AbstractSerializerFrom& serializer ) override;
    void                AbstractSerializable_serializeTo        ( AbstractSerializerTo& serializer ) const override;

public:

    //----------------------------------------------------------------//
    bool operator == ( const Block& rhs ) const {
        return ( this->mSignature == rhs.mSignature );
    }
    
    //----------------------------------------------------------------//
    bool operator != ( const Block& rhs ) const {
        return !( *this == rhs );
    }

    //----------------------------------------------------------------//
    bool                apply                               ( Ledger& ledger ) const;
    static int          compare                             ( const Block& block0, const Block& block1 );
                        Block                               ();
                        Block                               ( string minerID, size_t cycleID, const CryptoKey& key, string hashAlgorithm = Signature::DEFAULT_HASH_ALGORITHM );
                        ~Block                              ();
    size_t              countTransactions                   () const;
    size_t              getCycleID                          () const;
    size_t              getHeight                           () const;
    string              getMinerID                          () const;
    size_t              getScore                            () const;
    const Signature&    getSignature                        () const;
    void                pushTransaction                     ( shared_ptr < AbstractTransaction > transaction );
    void                setAllure                           ( const Digest& allure );
    void                setMinerID                          ( string minerID );
    const Digest&       sign                                ( const CryptoKey& key, string hashAlgorithm = Signature::DEFAULT_HASH_ALGORITHM );
};

} // namespace Volition
#endif
