// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_BLOCK_H
#define VOLITION_BLOCK_H

#include <volition/common.h>

#include <volition/AbstractTransaction.h>
#include <volition/Signature.h>
#include <volition/State.h>
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

    u64                             mHeight;
    u64                             mCycleID;
    string                          mMinerID;
    Poco::DigestEngine::Digest      mPrevDigest;
    Poco::DigestEngine::Digest      mAllure; // digital signature of the hash of mCycleID
    Signature                       mSignature;

    SerializableVector < SerializableSharedPtr < AbstractTransaction, TransactionFactory >>     mTransactions;

    //----------------------------------------------------------------//
    void                                setCycleID                          ( size_t cycleID );
    void                                setPreviousBlock                    ( const Block* prevBlock );
    bool                                verify                              ( const State& state, const Poco::Crypto::ECKey& key );

    //----------------------------------------------------------------//
    void                                AbstractSerializable_serialize      ( AbstractSerializer& serializer ) override;

public:

    //----------------------------------------------------------------//
    void                                apply                               ( State& state ) const;
                                        Block                               ();
                                        ~Block                              ();
    string                              getMinerID                          () const;
    size_t                              getScore                            () const;
    const Signature&                    getSignature                        () const;
    void                                pushTransaction                     ( shared_ptr < AbstractTransaction > transaction );
    void                                setMinerID                          ( string minerID );
    const Poco::DigestEngine::Digest&   sign                                ( const Poco::Crypto::ECKey& key, string hashAlgorithm = Signature::DEFAULT_HASH_ALGORITHM );
    bool                                verify                              ( const State& state );
};

} // namespace Volition
#endif
