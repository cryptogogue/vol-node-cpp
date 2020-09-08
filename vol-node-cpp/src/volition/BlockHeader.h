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

    static const size_t CHARM_SIZE      = 128;

    string              mMinerID;
    u64                 mHeight;
    SerializableTime    mTime;
    Digest              mDigest;
    Digest              mPrevDigest;
    Signature           mPose;
    Digest              mCharm;
    Signature           mSignature;

    //----------------------------------------------------------------//
    void                applyEntropy                        ( Ledger& ledger ) const;
    string              formatPoseString                  ( string prevPose ) const;
    Digest              hashPose                          ( string prevPose ) const;
    
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
                        BlockHeader                         ( string minerID, const Digest& visage, time_t now, const BlockHeader* prevBlock, const CryptoKey& key );
                        ~BlockHeader                        ();
    Digest              getCharm                            () const;
    static Digest       getCharm                            ( const Digest& pose, const Digest& visage );
    string              getHash                             () const;
    size_t              getHeight                           () const;
    string              getMinerID                          () const;
    Digest              getPose                             () const;
    string              getPrevHash                         () const;
    const Signature&    getSignature                        () const;
    time_t              getTime                             () const;
    bool                isGenesis                           () const;
    bool                isInRewriteWindow                   ( time_t now ) const;
    bool                isParent                            ( const BlockHeader& block ) const;
    void                setMinerID                          ( string minerID );
};

} // namespace Volition
#endif
