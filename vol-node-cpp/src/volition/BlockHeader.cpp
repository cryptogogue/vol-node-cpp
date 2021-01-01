// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/BlockHeader.h>
#include <volition/Format.h>
#include <volition/Ledger.h>
#include <volition/TheTransactionBodyFactory.h>

namespace Volition {

//================================================================//
// BlockHeaderFields
//================================================================//

//----------------------------------------------------------------//
BlockHeaderFields::BlockHeaderFields () :
    mHeight ( 0 ),
    mTime ( 0 ),
    mBlockDelay ( Ledger::DEFAULT_BLOCK_DELAY_IN_SECONDS ),
    mRewriteWindow ( Ledger::DEFAULT_REWRITE_WINDOW_IN_SECONDS ) {
}

//----------------------------------------------------------------//
BlockHeaderFields::~BlockHeaderFields () {
}

//================================================================//
// HasBlockHeaderFields
//================================================================//

//----------------------------------------------------------------//
Digest HasBlockHeaderFields::calculateCharm ( const Digest& pose, const Digest& visage ) {

    // CHARM = POSE ^ VISAGE

//    printf ( "POSE: %s\n", pose.toHex ().substr ( 0, 6 ).c_str ());
//    printf ( "VISAGE: %s\n", visage.toHex ().substr ( 0, 6 ).c_str ());

    Digest charm;
    charm.resize ( CHARM_SIZE );
    
    size_t poseSize     = pose.size ();
    size_t visageSize   = visage.size ();
    
    for ( size_t i = 0; i < CHARM_SIZE; ++i ) {
    
        u8 a = poseSize ? pose [ i % poseSize ] : 0;
        u8 v = visageSize ? visage [ i % visageSize ] : 0;
    
        charm [ i ] = a ^ v;
    }
    
//    printf ( "CHARM:  %s\n", charm.toHex ().c_str ());
    
    return charm;
}

//----------------------------------------------------------------//
string HasBlockHeaderFields::formatPoseString ( string prevPose ) const {

    const BlockHeaderFields& fields = this->getFields ();
    
    return Format::write ( "%s:%zu:%s", fields.mMinerID.c_str (), fields.mHeight, prevPose.c_str ());
}

//----------------------------------------------------------------//
string HasBlockHeaderFields::getCharmTag () const {

    return this->getCharm ().toHex ().substr ( 0, 6 );
}

//----------------------------------------------------------------//
const BlockHeaderFields& HasBlockHeaderFields::getFields () const {
    return HasBlockHeader_getFields ();
}

//----------------------------------------------------------------//
Digest HasBlockHeaderFields::getNextCharm ( const Digest& visage ) const {

    return HasBlockHeaderFields::calculateCharm ( this->getPose (), visage );
}

//----------------------------------------------------------------//
time_t HasBlockHeaderFields::getNextTime () const {

    const BlockHeaderFields& fields = this->getFields ();
    
    return fields.mTime + fields.mBlockDelay;
}

//----------------------------------------------------------------//
HasBlockHeaderFields::HasBlockHeaderFields () {
}

//----------------------------------------------------------------//
HasBlockHeaderFields::~HasBlockHeaderFields () {
}

//----------------------------------------------------------------//
Digest HasBlockHeaderFields::hashPose ( string prevPose ) const {

    return Digest ( this->formatPoseString ( prevPose ));
}

//----------------------------------------------------------------//
bool HasBlockHeaderFields::isGenesis () const {

    return ( this->getHeight () == 0 );
}

//----------------------------------------------------------------//
bool HasBlockHeaderFields::isInRewriteWindow ( time_t now ) const {

    const BlockHeaderFields& fields = this->getFields ();
    
    double diff = difftime ( now, fields.mTime );
    return diff < fields.mRewriteWindow;
}

//----------------------------------------------------------------//
bool HasBlockHeaderFields::isParent ( const BlockHeader& block ) const {

    return ( this->getDigest () == block.mPrevDigest ); // TODO: does not need to be constant time
}

//================================================================//
// BlockHeader
//================================================================//

//----------------------------------------------------------------//
void BlockHeader::applyEntropy ( Ledger& ledger ) const {

    ledger.setEntropyString ( this->mPose.toString ());
}

//----------------------------------------------------------------//
BlockHeader::BlockHeader () {
}

//----------------------------------------------------------------//
BlockHeader::~BlockHeader () {
}

//----------------------------------------------------------------//
int BlockHeader::compare ( const BlockHeader& block0, const BlockHeader& block1 ) {

    assert ( block0.mHeight == block1.mHeight );

    if ( block0.equals ( block1 )) return 0;
                
    return BlockHeader::compare ( block0.mCharm, block1.mCharm );
}

//----------------------------------------------------------------//
int BlockHeader::compare ( const Digest& charm0, const Digest& charm1 ) {

    string hex0 = charm0.toHex ();
    string hex1 = charm1.toHex ();

//    printf ( "charm0 (%s): %s\n", block0.getMinerID ().c_str (), hex0.c_str ());
//    printf ( "charm1 (%s): %s\n", block1.getMinerID ().c_str (), hex1.c_str ());
        
    int result = hex0.compare ( hex1 );
    return result < 0 ? -1 : result > 0 ? 1 : 0;
}

//----------------------------------------------------------------//
bool BlockHeader::equals ( const BlockHeader& rhs ) const {
    return (( this->mHeight == rhs.mHeight ) && ( this->mSignature == rhs.mSignature ));
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
