// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/BlockHeader.h>
#include <volition/Format.h>
#include <volition/Ledger.h>

namespace Volition {

//================================================================//
// BlockHeader
//================================================================//

//----------------------------------------------------------------//
void BlockHeader::applyEntropy ( AbstractLedger& ledger ) const {

    Digest entropy = Digest::hash ( this->mPose );
    ledger.setEntropyString ( entropy );
}

//----------------------------------------------------------------//
BlockHeader::BlockHeader () {
}

//----------------------------------------------------------------//
BlockHeader::~BlockHeader () {
}

//----------------------------------------------------------------//
void BlockHeader::initialize ( string minerID, const Digest& visage, time_t now, const BlockHeader* prevBlockHeader, const CryptoKeyPair& key ) {
        
    this->mMinerID      = minerID;
    this->mTime         = now;
    
    if ( prevBlockHeader ) {
        
        this->mHeight       = prevBlockHeader->mHeight + 1;
        this->mPrevDigest   = prevBlockHeader->mDigest;
        
        this->mPose         = key.sign ( this->hashPose ( prevBlockHeader->mPose.toHex ()), Digest::HASH_ALGORITHM_SHA256 );
        this->mCharm        = prevBlockHeader->getNextCharm ( visage );
    }
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
void BlockHeader::AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) {
    
    u64 blockDelay      = ( u64 )this->mBlockDelay;
    u64 rewriteWindow   = ( u64 )this->mRewriteWindow;
    
    serializer.serialize ( "height",            this->mHeight );
    serializer.serialize ( "version",           this->mVersion );
    serializer.serialize ( "time",              this->mTime );
    serializer.serialize ( "blockDelay",        blockDelay );
    serializer.serialize ( "rewriteWindow",     rewriteWindow );
    
    this->mBlockDelay       = ( time_t )blockDelay;
    this->mRewriteWindow    = ( time_t )rewriteWindow;
    
    if ( !this->isGenesis ()) {
    
        serializer.serialize ( "minerID",       this->mMinerID );
        serializer.serialize ( "prevDigest",    this->mPrevDigest );
        serializer.serialize ( "pose",          this->mPose );
        serializer.serialize ( "charm",         this->mCharm );
    }
    
    serializer.serialize ( "digest",            this->mDigest );
    serializer.serialize ( "signature",         this->mSignature );
}

//----------------------------------------------------------------//
void BlockHeader::AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const {
    
    u64 blockDelay      = ( u64 )this->mBlockDelay;
    u64 rewriteWindow   = ( u64 )this->mRewriteWindow;
    
    serializer.serialize ( "height",            this->mHeight );
    serializer.serialize ( "version",           this->mVersion );
    serializer.serialize ( "time",              this->mTime );
    serializer.serialize ( "blockDelay",        blockDelay );
    serializer.serialize ( "rewriteWindow",     rewriteWindow );
    
    if ( !this->isGenesis ()) {
        
        serializer.serialize ( "minerID",       this->mMinerID );
        serializer.serialize ( "prevDigest",    this->mPrevDigest );
        serializer.serialize ( "pose",          this->mPose );
        serializer.serialize ( "charm",         this->mCharm );
    }
    
    if ( !serializer.isDigest ()) {
        serializer.serialize ( "digest",        this->mDigest );
        serializer.serialize ( "signature",     this->mSignature );
    }
}

//----------------------------------------------------------------//
const BlockHeaderFields& BlockHeader::HasBlockHeader_getFields () const {

    return *this;
}

} // namespace Volition
