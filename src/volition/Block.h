// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_BLOCK_H
#define VOLITION_BLOCK_H

#include "common.h"

#include "AbstractSerializable.h"
#include "AbstractTransaction.h"
#include "Signature.h"
#include "State.h"

namespace Volition {

//================================================================//
// Block
//================================================================//
class Block :
    public AbstractHashable,
    public AbstractSerializable {
private:

    friend class Context;
    friend class Chain;

    u64                             mHeight;
    u64                             mCycleID;
    string                          mMinerID;
    Poco::DigestEngine::Digest      mPrevDigest;
    Poco::DigestEngine::Digest      mAllure; // digital signature of the hash of mCycleID
    Signature                       mSignature;

    vector < shared_ptr < const AbstractTransaction >>  mTransactions;

    //----------------------------------------------------------------//
    void                                setCycleID                          ( size_t cycleID );
    void                                setPreviousBlock                    ( const Block* prevBlock );
    bool                                verify                              ( const State& state, const Poco::Crypto::ECKey& key ) const;

    //----------------------------------------------------------------//
    void                                AbstractHashable_hash               ( Poco::DigestOutputStream& digestStream ) const override;
    void                                AbstractSerializable_fromJSON       ( const Poco::JSON::Object& object ) override;
    void                                AbstractSerializable_toJSON         ( Poco::JSON::Object& object ) const override;

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
    bool                                verify                              ( const State& state ) const;
};

} // namespace Volition
#endif
