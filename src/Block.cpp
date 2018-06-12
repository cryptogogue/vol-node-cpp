//
//  main.cpp
// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include "Block.h"
#include "TheTransactionFactory.h"

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
    mCycleID ( 0 ) {
}

//----------------------------------------------------------------//
Block::~Block () {
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
void Block::setCycleID ( size_t cycleID ) {

    this->mCycleID = cycleID;
}

//----------------------------------------------------------------//
void Block::setMinerID ( string minerID ) {

    this->mMinerID = minerID;
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
void Block::AbstractHashable_hash ( Poco::DigestOutputStream& digestStream ) const {

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

    Signable::AbstractSerializable_fromJSON ( object );
    
    string prevDigestString         = object.optValue < string >( "prevDigest", "" );
    string allureString             = object.optValue < string >( "allure", "" );
    
    this->mCycleID                  = object.optValue < int >( "cycleID", 0 );
    this->mMinerID                  = object.optValue < string >( "minerID", "" );
    this->mPrevDigest               = prevDigestString.size () ? Poco::DigestEngine::digestFromHex ( prevDigestString ) : Poco::DigestEngine::Digest ();
    this->mAllure                   = allureString.size () ? Poco::DigestEngine::digestFromHex ( allureString ) : Poco::DigestEngine::Digest ();
    
    const Poco::JSON::Array::Ptr transactions = object.getArray ( "transactions" );
    this->mTransactions.resize ( transactions->size ());
    
    for ( size_t i = 0; i < transactions->size (); ++i ) {
        const Poco::JSON::Object::Ptr transaction = transactions->getObject (( unsigned int )i );
        this->mTransactions [ i ].reset ( TheTransactionFactory::get ().create ( *transaction ));
    }
}

//----------------------------------------------------------------//
void Block::AbstractSerializable_toJSON ( Poco::JSON::Object& object ) const {

    Signable::AbstractSerializable_toJSON ( object );
    
    object.set ( "cycleID",             this->mCycleID );
    object.set ( "minerID",             this->mMinerID.c_str ());
    object.set ( "prevDigest",          Poco::DigestEngine::digestToHex ( this->mPrevDigest ).c_str ());
    object.set ( "allure",              Poco::DigestEngine::digestToHex ( this->mAllure ).c_str ());
    
    Poco::JSON::Array::Ptr transactions = new Poco::JSON::Array ();
    object.set ( "transactions", transactions );
    
    for ( size_t i = 0; i < this->mTransactions.size (); ++i ) {
        const AbstractTransaction& transaction = *this->mTransactions [ i ];
        transactions->set (( unsigned int )i, transaction.toJSON ());
    }
}

//----------------------------------------------------------------//
const Poco::DigestEngine::Digest& Block::Signable_sign ( const Poco::Crypto::ECKey& key, string hashAlgorithm ) {

    Poco::Crypto::ECDSADigestEngine signature ( key, hashAlgorithm );
    Poco::DigestOutputStream signatureStream ( signature );
    signatureStream << this->mCycleID;
    signatureStream.close ();
    
    this->mAllure = signature.signature ();

    return Signable::Signable_sign ( key, hashAlgorithm );
}

//----------------------------------------------------------------//
bool Block::Signable_verify ( const Poco::Crypto::ECKey& key, string hashAlgorithm ) const {

    Poco::Crypto::ECDSADigestEngine signature ( key, hashAlgorithm );
    Poco::DigestOutputStream signatureStream ( signature );
    signatureStream << this->mCycleID;
    signatureStream.close ();

    return signature.verify ( this->mAllure ) && Signable::Signable_verify ( key, hashAlgorithm );
}

} // namespace Volition
