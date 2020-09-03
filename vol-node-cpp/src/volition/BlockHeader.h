// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_BLOCKHEADER_H
#define VOLITION_BLOCKHEADER_H

#include <volition/common.h>

#include <volition/CryptoKey.h>
#include <volition/serialization/Serialization.h>
#include <volition/Signature.h>

namespace Volition {

class Ledger;

//================================================================//
// BlockHeader
//================================================================//
class BlockHeader :
    public AbstractSerializable {
protected:

    string              mMinerID;
    u64                 mHeight;
    SerializableTime    mTime;
    Digest              mPrevDigest;
    Digest              mAllure; // digital signature of the hash of block height
    Signature           mSignature;

    //----------------------------------------------------------------//
    void                applyEntropy                        ( Ledger& ledger ) const;
    void                computeAllure                       ( Poco::Crypto::ECDSADigestEngine& signature ) const;
    void                setPreviousBlock                    ( const BlockHeader& prevBlock );
    
    //----------------------------------------------------------------//
    void                AbstractSerializable_serializeFrom      ( const AbstractSerializerFrom& serializer ) override;
    void                AbstractSerializable_serializeTo        ( AbstractSerializerTo& serializer ) const override;

public:
    
    //----------------------------------------------------------------//
    bool operator == ( const BlockHeader& rhs ) const {
        return ( this->mSignature == rhs.mSignature );
    }
    
    //----------------------------------------------------------------//
    bool operator != ( const BlockHeader& rhs ) const {
        return !( *this == rhs );
    }

    //----------------------------------------------------------------//
    static int          compare                             ( const BlockHeader& block0, const BlockHeader& block1 );
                        BlockHeader                         ();
                        BlockHeader                         ( string minerID, time_t now, const BlockHeader* prevBlock, const CryptoKey& key, string hashAlgorithm = Digest::DEFAULT_HASH_ALGORITHM );
                        ~BlockHeader                        ();
    string              getHash                             () const;
    size_t              getHeight                           () const;
    string              getMinerID                          () const;
    string              getPrevHash                         () const;
    size_t              getScore                            () const;
    const Signature&    getSignature                        () const;
    time_t              getTime                             () const;
    bool                isGenesis                           () const;
    bool                isInRewriteWindow                   ( time_t now ) const;
    bool                isParent                            ( const BlockHeader& block ) const;
    void                setAllure                           ( const Digest& allure );
    void                setMinerID                          ( string minerID );
};

} // namespace Volition
#endif
