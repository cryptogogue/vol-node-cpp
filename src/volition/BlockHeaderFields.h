// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_BLOCKHEADERFIELDS_H
#define VOLITION_BLOCKHEADERFIELDS_H

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
    u64                 mVersion;
    
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

} // namespace Volition
#endif
