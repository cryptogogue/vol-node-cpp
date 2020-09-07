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

    ledger.setEntropyString ( this->mAllure.toString ());
}

//----------------------------------------------------------------//
BlockHeader::BlockHeader () :
    mHeight ( 0 ),
    mTime ( 0 ) {
}

//----------------------------------------------------------------//
BlockHeader::BlockHeader ( string minerID, time_t now, const BlockHeader* prevBlockHeader, const CryptoKey& key, string hashAlgorithm ) :
    mMinerID ( minerID ),
    mHeight ( 0 ),
    mTime ( now ) {
        
    if ( prevBlockHeader ) {
        
        this->mHeight = prevBlockHeader->mHeight + 1;
        this->mPrevDigest = prevBlockHeader->mDigest;
        
        Digest digest ( prevBlockHeader->mAllure.toHex (), hashAlgorithm );
        this->mAllure = key.sign ( digest, hashAlgorithm );
    }
}

//----------------------------------------------------------------//
BlockHeader::~BlockHeader () {
}

//----------------------------------------------------------------//
int BlockHeader::compare ( const BlockHeader& block0, const BlockHeader& block1 ) {

    assert ( block0.mHeight == block1.mHeight );

    if ( block0 == block1 ) return 0;
                
    string allure0 = Poco::DigestEngine::digestToHex ( block0.mAllure );
    string allure1 = Poco::DigestEngine::digestToHex ( block1.mAllure );

//    printf ( "allure0 (%s): %s\n", block0.getMinerID ().c_str (), allure0.c_str ());
//    printf ( "allure1 (%s): %s\n", block1.getMinerID ().c_str (), allure1.c_str ());
        
    int result = allure0.compare ( allure1 );
    return result < 0 ? -1 : result > 0 ? 1 : 0;
}

//----------------------------------------------------------------//
string BlockHeader::getAllure () const {

    return this->mAllure;
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

//----------------------------------------------------------------//
void BlockHeader::setPreviousBlock ( const BlockHeader& prevBlockHeader ) {

    this->mHeight = prevBlockHeader.mHeight + 1;
    this->mPrevDigest = prevBlockHeader.mDigest;
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
        serializer.serialize ( "allure",        this->mAllure );
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
        serializer.serialize ( "allure",        this->mAllure );
    }
    
    if ( !serializer.isDigest ()) {
        serializer.serialize ( "digest",        this->mDigest );
        serializer.serialize ( "signature",     this->mSignature );
    }
}

} // namespace Volition
