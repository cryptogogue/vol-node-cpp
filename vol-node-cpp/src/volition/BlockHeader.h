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
    static Digest       calculateCharm                      ( const Digest& pose, const Digest& visage );
    string              formatPoseString                    ( string prevPose ) const;
    Digest              hashPose                            ( string prevPose ) const;
    
    //----------------------------------------------------------------//
    void                AbstractSerializable_serializeFrom      ( const AbstractSerializerFrom& serializer ) override;
    void                AbstractSerializable_serializeTo        ( AbstractSerializerTo& serializer ) const override;

public:
    
    //----------------------------------------------------------------//
    bool operator == ( const BlockHeader& rhs ) const {
        return (( this->mHeight == rhs.mHeight ) && ( this->mSignature == rhs.mSignature ));
    }
    
    //----------------------------------------------------------------//
    bool operator != ( const BlockHeader& rhs ) const {
        return !( *this == rhs );
    }

    //----------------------------------------------------------------//
    static int          compare                             ( const BlockHeader& block0, const BlockHeader& block1 );
    static int          compare                             ( const Digest& charm0, const Digest& charm1 );
                        BlockHeader                         ();
                        BlockHeader                         ( string minerID, const Digest& visage, time_t now, const BlockHeader* prevBlock, const CryptoKey& key );
                        ~BlockHeader                        ();
    const Digest&       getCharm                            () const;
    string              getHash                             () const;
    size_t              getHeight                           () const;
    string              getMinerID                          () const;
    Digest              getNextCharm                        ( const Digest& visage ) const;
    const Digest&       getPose                             () const;
    string              getPrevHash                         () const;
    const Signature&    getSignature                        () const;
    time_t              getTime                             () const;
    bool                isGenesis                           () const;
    bool                isInRewriteWindow                   () const;
    bool                isInRewriteWindow                   ( time_t now ) const;
    bool                isParent                            ( const BlockHeader& block ) const;
    void                setMinerID                          ( string minerID );
};

} // namespace Volition
#endif
