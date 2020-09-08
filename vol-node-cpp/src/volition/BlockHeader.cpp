// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/BlockHeader.h>
#include <volition/Format.h>
#include <volition/Ledger.h>
#include <volition/TheContext.h>
#include <volition/TheTransactionBodyFactory.h>

namespace Volition {

//================================================================//
// BlockHeader
//================================================================//

//----------------------------------------------------------------//
void BlockHeader::applyEntropy ( Ledger& ledger ) const {

    ledger.setEntropyString ( this->mPose.toString ());
}

//----------------------------------------------------------------//
BlockHeader::BlockHeader () :
    mHeight ( 0 ),
    mTime ( 0 ) {
}

//----------------------------------------------------------------//
BlockHeader::BlockHeader ( string minerID, const Digest& visage, time_t now, const BlockHeader* prevBlockHeader, const CryptoKey& key ) :
    mMinerID ( minerID ),
    mHeight ( 0 ),
    mTime ( now ) {
        
    if ( prevBlockHeader ) {
        
        this->mHeight       = prevBlockHeader->mHeight + 1;
        this->mPrevDigest   = prevBlockHeader->mDigest;
        
        this->mPose       = key.sign ( this->hashPose ( prevBlockHeader->mPose.toHex ()), Digest::HASH_ALGORITHM_SHA256 );
        this->mCharm        = BlockHeader::getCharm ( this->mPose, visage );
    }
}

//----------------------------------------------------------------//
BlockHeader::~BlockHeader () {
}

//----------------------------------------------------------------//
int BlockHeader::compare ( const BlockHeader& block0, const BlockHeader& block1 ) {

    assert ( block0.mHeight == block1.mHeight );

    if ( block0 == block1 ) return 0;
                
    string charm0 = Poco::DigestEngine::digestToHex ( block0.mCharm );
    string charm1 = Poco::DigestEngine::digestToHex ( block1.mCharm );

//    printf ( "charm0 (%s): %s\n", block0.getMinerID ().c_str (), charm0.c_str ());
//    printf ( "charm1 (%s): %s\n", block1.getMinerID ().c_str (), charm1.c_str ());
        
    int result = charm0.compare ( charm1 );
    return result < 0 ? -1 : result > 0 ? 1 : 0;
}

//----------------------------------------------------------------//
string BlockHeader::formatPoseString ( string prevPose ) const {

    return Format::write ( "%s:%zu:%s", this->mMinerID.c_str (), this->mHeight, prevPose.c_str ());
}

//----------------------------------------------------------------//
Digest BlockHeader::getCharm () const {

    return this->mCharm;
}

//----------------------------------------------------------------//
Digest BlockHeader::getCharm ( const Digest& pose, const Digest& visage ) {

    // CHARM = POSE ^ VISAGE

//    printf ( "POSE: %s\n", pose.toHex ().c_str ());
//    printf ( "VISAGE: %s\n", visage.toHex ().c_str ());

    Digest charm;
    charm.resize ( CHARM_SIZE );
    
    size_t poseSize   = pose.size ();
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
string BlockHeader::getHash () const {

    return this->mDigest;
}

//----------------------------------------------------------------//
size_t BlockHeader::getHeight () const {

    return this->mHeight;
}

//----------------------------------------------------------------//
string BlockHeader::getMinerID () const {

    return this->mMinerID;
}

//----------------------------------------------------------------//
Digest BlockHeader::getPose () const {

    return this->mPose;
}

//----------------------------------------------------------------//
string BlockHeader::getPrevHash () const {

    return this->mPrevDigest.toString ();
}

//----------------------------------------------------------------//
const Signature& BlockHeader::getSignature () const {

    return this->mSignature;
}

//----------------------------------------------------------------//
time_t BlockHeader::getTime () const {

    return this->mTime;
}

//----------------------------------------------------------------//
Digest BlockHeader::hashPose ( string prevPose ) const {

    return Digest ( this->formatPoseString ( prevPose ));
}

//----------------------------------------------------------------//
bool BlockHeader::isGenesis () const {

    return ( this->mHeight == 0 );
}

//----------------------------------------------------------------//
bool BlockHeader::isInRewriteWindow ( time_t now ) const {

    double window = TheContext::get ().getWindow ();
    double diff = difftime ( now, this->mTime );
    return diff < window;
}

//----------------------------------------------------------------//
bool BlockHeader::isParent ( const BlockHeader& block ) const {

    return ( this->mDigest == block.mPrevDigest ); // TODO: does not need to be constant time
}

//----------------------------------------------------------------//
void BlockHeader::setMinerID ( string minerID ) {

    this->mMinerID = minerID;
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
void BlockHeader::AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) {
    
    serializer.serialize ( "height",        this->mHeight );
    serializer.serialize ( "time",          this->mTime );
    
    if ( !this->isGenesis ()) {
    
        serializer.serialize ( "minerID",       this->mMinerID );
        serializer.serialize ( "prevDigest",    this->mPrevDigest );
        serializer.serialize ( "pose",        this->mPose );
        serializer.serialize ( "charm",         this->mCharm );
    }
    
    serializer.serialize ( "digest",        this->mDigest );
    serializer.serialize ( "signature",     this->mSignature );
}

//----------------------------------------------------------------//
void BlockHeader::AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const {
    
    serializer.serialize ( "height",        this->mHeight );
    serializer.serialize ( "time",          this->mTime );
    
    if ( !this->isGenesis ()) {
        
        serializer.serialize ( "minerID",       this->mMinerID );
        serializer.serialize ( "prevDigest",    this->mPrevDigest );
        serializer.serialize ( "pose",        this->mPose );
        serializer.serialize ( "charm",         this->mCharm );
    }
    
    if ( !serializer.isDigest ()) {
        serializer.serialize ( "digest",        this->mDigest );
        serializer.serialize ( "signature",     this->mSignature );
    }
}

} // namespace Volition
