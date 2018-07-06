//
//  main.cpp
// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Block.h>
#include <volition/Serialize.h>
#include <volition/TheContext.h>
#include <volition/TheTransactionFactory.h>

namespace Volition {

//================================================================//
// Block
//================================================================//

//----------------------------------------------------------------//
void Block::apply ( State& state ) const {

    for ( size_t i = 0; i < this->mTransactions.size (); ++i ) {
        const AbstractTransaction& transaction = *this->mTransactions [ i ];
        transaction.apply ( state );
    }
}

//----------------------------------------------------------------//
Block::Block () :
    mCycleID ( 0 ),
    mHeight ( 0 ) {
}

//----------------------------------------------------------------//
Block::~Block () {
}

//----------------------------------------------------------------//
const Signature& Block::getSignature () const {

    return this->mSignature;
}

//----------------------------------------------------------------//
string Block::getMinerID () const {

    return this->mMinerID;
}

//----------------------------------------------------------------//
size_t Block::getScore () const {

    string allureString = Poco::DigestEngine::digestToHex ( this->mAllure );
    return std::hash < string >{}( allureString );
}

//----------------------------------------------------------------//
void Block::pushTransaction ( shared_ptr < AbstractTransaction > transaction ) {

    this->mTransactions.push_back ( transaction );
}

//----------------------------------------------------------------//
void Block::setCycleID ( size_t cycleID ) {

    this->mCycleID = cycleID;
}

//----------------------------------------------------------------//
void Block::setMinerID ( string minerID ) {

    this->mMinerID = minerID;
}

//----------------------------------------------------------------//
void Block::setPreviousBlock ( const Block* prevBlock ) {

    if ( prevBlock ) {
        this->mHeight = prevBlock->mHeight + 1;
        this->mPrevDigest = prevBlock->mSignature.getDigest ();
    }
}

//----------------------------------------------------------------//
const Poco::DigestEngine::Digest& Block::sign ( const Poco::Crypto::ECKey& key, string hashAlgorithm ) {

    // no need to compute allure for the genesis block
    if ( this->mHeight ) {

        Poco::Crypto::ECDSADigestEngine signature ( key, hashAlgorithm );
        Poco::DigestOutputStream signatureStream ( signature );
        signatureStream << this->mCycleID;
        signatureStream.close ();
        
        this->mAllure = signature.signature ();
    }

    return this->mSignature.sign ( *this, key, hashAlgorithm );
}

//----------------------------------------------------------------//
bool Block::verify ( const State& state ) const {

    const MinerInfo* minerInfo = state.getMinerInfo ( this->mMinerID );
    
    if ( minerInfo ) {
        return this->verify ( state, minerInfo->getPublicKey ());
    }
    
    // no miner info; must be the genesis block
    
    if ( this->mHeight > 0 ) return false; // genesis block must be height 0

    // check that it's the expected genesis block
    if ( !Poco::DigestEngine::constantTimeEquals ( TheContext::get ().getGenesisBlockDigest (), this->getSignature ().getDigest ())) return false;

    return this->verify ( state, TheContext::get ().getGenesisBlockKey ());
}

//----------------------------------------------------------------//
bool Block::verify ( const State& state, const Poco::Crypto::ECKey& key ) const {

    if ( this->mHeight > 0 ) {
    
        string hashAlgorithm = this->mSignature.getHashAlgorithm ();

        Poco::Crypto::ECDSADigestEngine signature ( key, hashAlgorithm );
        Poco::DigestOutputStream signatureStream ( signature );
        signatureStream << this->mCycleID;
        signatureStream.close ();
        
        if ( !signature.verify ( this->mAllure )) {
            return false;
        }
    }

    // TODO: verify transactions, too!

    return this->mSignature.verify ( *this, key );
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
void Block::AbstractHashable_hash ( Poco::DigestOutputStream& digestStream ) const {

    digestStream << this->mHeight;
    digestStream << this->mCycleID;
    digestStream << this->mMinerID;
    digestStream << Poco::DigestEngine::digestToHex ( this->mPrevDigest );
    digestStream << Poco::DigestEngine::digestToHex ( this->mAllure );
    
    for ( size_t i = 0; i < this->mTransactions.size (); ++i ) {
        this->mTransactions [ i ]->hash ( digestStream );
    }
}

//----------------------------------------------------------------//
void Block::AbstractSerializable_fromJSON ( const Poco::JSON::Object& object ) {
    
    this->mMinerID                  = object.optValue < string >( "minerID", "" );
    
    this->mHeight                   = Serialize::getU64FromJSON ( object, "height", 0 );
    this->mCycleID                  = Serialize::getU64FromJSON ( object, "cycleID", 0 );
    
    Serialize::getSerializableFromJSON ( object, "prevDigest", this->mPrevDigest );
    Serialize::getSerializableFromJSON ( object, "allure", this->mAllure );
    Serialize::getSerializableFromJSON ( object, "signature", this->mSignature );
    
    const Poco::JSON::Array::Ptr transactions = object.getArray ( "transactions" );
    this->mTransactions.resize ( transactions->size ());
    
    for ( size_t i = 0; i < transactions->size (); ++i ) {
        const Poco::JSON::Object::Ptr transaction = transactions->getObject (( unsigned int )i );
        this->mTransactions [ i ].reset ( TheTransactionFactory::get ().create ( *transaction ));
    }
}

//----------------------------------------------------------------//
void Block::AbstractSerializable_toJSON ( Poco::JSON::Object& object ) const {
    
    Serialize::setU64ToJSON ( object, "height",     this->mHeight );
    Serialize::setU64ToJSON ( object, "cycleID",    this->mCycleID );
    
    if ( this->mHeight > 0 ) {
    
        object.set ( "minerID",             this->mMinerID.c_str ());

        Serialize::setSerializableToJSON ( object, "prevDigest",    this->mPrevDigest );
        Serialize::setSerializableToJSON ( object, "allure",        this->mAllure );
    }
    
    Serialize::setSerializableToJSON ( object, "signature",     this->mSignature );
    
    Poco::JSON::Array::Ptr transactions = new Poco::JSON::Array ();
    object.set ( "transactions", transactions );
    
    for ( size_t i = 0; i < this->mTransactions.size (); ++i ) {
        const AbstractTransaction& transaction = *this->mTransactions [ i ];
        transactions->set (( unsigned int )i, transaction.toJSON ());
    }
}

} // namespace Volition
