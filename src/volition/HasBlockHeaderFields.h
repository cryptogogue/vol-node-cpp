// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_HASBLOCKHEADERFIELDS_H
#define VOLITION_HASBLOCKHEADERFIELDS_H

#include <volition/common.h>

#include <volition/Accessors.h>
#include <volition/BlockHeaderFields.h>

namespace Volition {

class BlockHeader;
class Ledger;

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
    GET ( size_t,                   Version,                    this->getFields ().mVersion )
    GET ( string,                   MinerID,                    this->getFields ().mMinerID )
    GET ( const Digest&,            Pose,                       this->getFields ().mPose )
    GET ( const Digest&,            PrevDigest,                 this->getFields ().mPrevDigest )
    GET ( time_t,                   RewriteWindow,              this->getFields ().mRewriteWindow )
    GET ( const Signature&,         Signature,                  this->getFields ().mSignature )
    GET ( time_t,                   Time,                       this->getFields ().mTime )
    
    //----------------------------------------------------------------//
    static int                      compare                         ( const HasBlockHeaderFields& block0, const HasBlockHeaderFields& block1 );
    static int                      compare                         ( const Digest& charm0, const Digest& charm1 );
    bool                            equals                          ( const HasBlockHeaderFields& rhs ) const;
    string                          getCharmTag                     () const;
    const BlockHeaderFields&        getFields                       () const;
    Digest                          getNextCharm                    ( const Digest& visage ) const;
    time_t                          getNextTime                     () const;
                                    HasBlockHeaderFields            ();
    virtual                         ~HasBlockHeaderFields           ();
    bool                            isGenesis                       () const;
    bool                            isInRewriteWindow               ( time_t now ) const;
    bool                            isParent                        ( const HasBlockHeaderFields& block ) const;
};

} // namespace Volition
#endif
