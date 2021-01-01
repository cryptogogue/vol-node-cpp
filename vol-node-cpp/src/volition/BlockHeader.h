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

class BlockHeader;
class Ledger;

//================================================================//
// BlockHeaderFields
//================================================================//
class BlockHeaderFields :
    public virtual AbstractSerializable {
protected:

    friend class HasBlockHeaderFields;

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
                        BlockHeaderFields           ();
                        ~BlockHeaderFields          ();
};

//================================================================//
// HasBlockHeaderFields
//================================================================//
class HasBlockHeaderFields {
protected:

    static const size_t CHARM_SIZE  = 128;

    //----------------------------------------------------------------//
    static Digest                   calculateCharm                  ( const Digest& pose, const Digest& visage );
    string                          formatPoseString                ( string prevPose ) const;
    Digest                          hashPose                        ( string prevPose ) const;

    //----------------------------------------------------------------//
    virtual const BlockHeaderFields&        HasBlockHeader_getFields        () const = 0;

public:

    GET ( time_t,                   BlockDelayInSeconds,        this->getFields ().mBlockDelay )
    GET ( const Digest&,            Charm,                      this->getFields ().mCharm )
    GET ( const Digest&,            Digest,                     this->getFields ().mDigest )
    GET ( size_t,                   Height,                     this->getFields ().mHeight )
    GET ( string,                   MinerID,                    this->getFields ().mMinerID )
    GET ( const Digest&,            Pose,                       this->getFields ().mPose )
    GET ( const Digest&,            PrevDigest,                 this->getFields ().mPrevDigest )
    GET ( time_t,                   RewriteWindow,              this->getFields ().mRewriteWindow )
    GET ( const Signature&,         Signature,                  this->getFields ().mSignature )
    GET ( time_t,                   Time,                       this->getFields ().mTime )
    
    //----------------------------------------------------------------//
    string                          getCharmTag                     () const;
    const BlockHeaderFields&        getFields                       () const;
    Digest                          getNextCharm                    ( const Digest& visage ) const;
    time_t                          getNextTime                     () const;
                                    HasBlockHeaderFields            ();
    virtual                         ~HasBlockHeaderFields           ();
    bool                            isGenesis                       () const;
    bool                            isInRewriteWindow               ( time_t now ) const;
    bool                            isParent                        ( const BlockHeader& block ) const;
};

//================================================================//
// BlockHeader
//================================================================//
class BlockHeader :
    public BlockHeaderFields,
    public HasBlockHeaderFields {
protected:

    //----------------------------------------------------------------//
    void                applyEntropy                        ( Ledger& ledger ) const;
    
    //----------------------------------------------------------------//
    void                            AbstractSerializable_serializeFrom      ( const AbstractSerializerFrom& serializer ) override;
    void                            AbstractSerializable_serializeTo        ( AbstractSerializerTo& serializer ) const override;
    const BlockHeaderFields&        HasBlockHeader_getFields                () const override;

public:

    SET ( time_t,               BlockDelayInSeconds,    mBlockDelay )
    SET ( const Digest&,        Charm,                  mCharm )
    SET ( const Digest&,        Digest,                 mDigest )
    SET ( string,               MinerID,                mMinerID )
    SET ( time_t,               RewriteWindow,          mRewriteWindow )

    //----------------------------------------------------------------//
                        BlockHeader                         ();
                        ~BlockHeader                        ();
    static int          compare                             ( const BlockHeader& block0, const BlockHeader& block1 );
    static int          compare                             ( const Digest& charm0, const Digest& charm1 );
    bool                equals                              ( const BlockHeader& rhs ) const;
    void                initialize                          ( string minerID, const Digest& visage, time_t now, const BlockHeader* prevBlock, const CryptoKeyPair& key );
};

} // namespace Volition
#endif
