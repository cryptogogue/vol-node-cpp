// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_BLOCK_H
#define VOLITION_BLOCK_H

#include "common.h"

#include "AbstractSerializable.h"
#include "AbstractTransaction.h"
#include "Signable.h"
#include "State.h"

namespace Volition {

//================================================================//
// Block
//================================================================//
class Block :
    public Signable {
private:

    friend class Context;

    size_t                          mCycleID;
    string                          mMinerID;
    Poco::DigestEngine::Digest      mPrevDigest;
    Poco::DigestEngine::Digest      mAllure; // digital signature of the hash of mCycleID

    vector < unique_ptr < AbstractTransaction >>  mTransactions;

    //----------------------------------------------------------------//
    void                                    AbstractHashable_hash               ( Poco::DigestOutputStream& digestStream ) const override;
    void                                    AbstractSerializable_fromJSON       ( const Poco::JSON::Object& object ) override;
    void                                    AbstractSerializable_toJSON         ( Poco::JSON::Object& object ) const override;
    const Poco::DigestEngine::Digest&       Signable_sign                       ( const Poco::Crypto::ECKey& key, string hashAlgorithm ) override;
    bool                                    Signable_verify                     ( const Poco::Crypto::ECKey& key, string hashAlgorithm ) const override;

public:

    //----------------------------------------------------------------//
    void                apply               ( State& state ) const;
                        Block               ();
                        ~Block              ();
    string              getMinerID          () const;
    size_t              getScore            () const;
    void                setCycleID          ( size_t cycleID );
    void                setMinerID          ( string minerID );
};

} // namespace Volition
#endif
