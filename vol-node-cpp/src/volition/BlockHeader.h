// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_BLOCKHEADER_H
#define VOLITION_BLOCKHEADER_H

#include <volition/common.h>

#include <volition/Accessors.h>
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
    SerializableTime    mBlockDelay;
    SerializableTime    mRewriteWindow;
    
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
    
    GET ( size_t,               Height,                 mHeight )
    GET ( time_t,               NextTime,               ( mTime + mBlockDelay ))
    GET ( const Digest&,        Pose,                   mPose )
    GET ( const Digest&,        PrevDigest,             mPrevDigest )
    GET ( const Signature&,     Signature,              mSignature )
    GET ( time_t,               Time,                   mTime )

    GET_SET ( time_t,           BlockDelayInSeconds,    mBlockDelay )
    GET_SET ( const Digest&,    Charm,                  mCharm )
    GET_SET ( const Digest&,    Digest,                 mDigest )
    GET_SET ( string,           MinerID,                mMinerID )
    GET_SET ( time_t,           RewriteWindow,          mRewriteWindow )
    
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
                        BlockHeader                         ( string minerID, const Digest& visage, time_t now, const BlockHeader* prevBlock, const CryptoKeyPair& key );
                        ~BlockHeader                        ();
    Digest              getNextCharm                        ( const Digest& visage ) const;
    bool                isGenesis                           () const;
    bool                isInRewriteWindow                   ( time_t now ) const;
    bool                isParent                            ( const BlockHeader& block ) const;
};

} // namespace Volition
#endif
