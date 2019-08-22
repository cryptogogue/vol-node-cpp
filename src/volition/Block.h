// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_BLOCK_H
#define VOLITION_BLOCK_H

#include <volition/common.h>

#include <volition/Entropy.h>
#include <volition/Ledger.h>
#include <volition/serialization/Serialization.h>
#include <volition/Signature.h>
#include <volition/Transaction.h>

namespace Volition {

//================================================================//
// Block
//================================================================//
class Block :
    public AbstractSerializable {
private:

    friend class Context;
    friend class Chain;
    friend class Ledger;

    string          mMinerID;
    u64             mHeight;
    time_t          mTime;
    Digest          mPrevDigest;
    Digest          mAllure; // digital signature of the hash of mCycleID
    Signature       mSignature;

    // TODO: store these in a map indexed by maturity (so we don't have to traverse all transactions when handling deferred transactions)
    SerializableVector < SerializableSharedPtr < Transaction >>     mTransactions;

    //----------------------------------------------------------------//
    void                applyEntropy                        ( Ledger& ledger ) const;
    size_t              applyTransactions                   ( Ledger& ledger ) const;
    void                computeAllure                       ( Poco::Crypto::ECDSADigestEngine& signature ) const;
    void                setPreviousBlock                    ( const Block& prevBlock );
    bool                verify                              ( const Ledger& ledger ) const;
    bool                verify                              ( const CryptoKey& key ) const;

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
                        Block                               ( string minerID, time_t now, const Block* prevBlock, const CryptoKey& key, string hashAlgorithm = Signature::DEFAULT_HASH_ALGORITHM );
                        ~Block                              ();
    size_t              countTransactions                   () const;
    size_t              getHeight                           () const;
    string              getMinerID                          () const;
    size_t              getScore                            () const;
    const Signature&    getSignature                        () const;
    u64                 getTime                             () const;
    bool                isInRewriteWindow                   ( time_t now ) const;
    bool                isParent                            ( const Block& block ) const;
    void                pushTransaction                     ( shared_ptr < Transaction > transaction );
    void                setAllure                           ( const Digest& allure );
    void                setMinerID                          ( string minerID );
    const Digest&       sign                                ( const CryptoKey& key, string hashAlgorithm = Signature::DEFAULT_HASH_ALGORITHM );
};

} // namespace Volition
#endif
