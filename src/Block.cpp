//
//  main.cpp
//  consensus
//
//  Created by Patrick Meehan on 4/27/18.
//  Copyright © 2018 Patrick Meehan. All rights reserved.
//

#include "Block.h"

namespace Volition {

//================================================================//
// Block
//================================================================//

//----------------------------------------------------------------//
Block::Block () {
}

//----------------------------------------------------------------//
Block::~Block () {
}

//----------------------------------------------------------------//
void Block::sign () {

    Poco::Crypto::DigestEngine sha256 ( "SHA256" );
    Poco::DigestOutputStream digestStream ( sha256 );
    this->hash ( digestStream );
    digestStream.close ();
    
    this->mBlockHash = Poco::DigestEngine::digestToHex ( sha256.digest ());
    printf ( "HASH: %s\n", this->mBlockHash.c_str ());
    
    Poco::Crypto::ECKey key ( "secp256k1" );
    Poco::Crypto::ECDSADigestEngine signature ( key, "SHA256" );
    Poco::DigestOutputStream signatureStream ( signature );
    this->hash ( signatureStream );
    signatureStream.close ();
    
    string hash = Poco::DigestEngine::digestToHex ( signature.digest ());
    printf ( "HASH: %s\n", hash.c_str ());
    
    string sig = Poco::DigestEngine::digestToHex ( signature.signature ());
    printf ( "SIG: %s\n", sig.c_str ());
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
void Block::AbstractHashable_hash ( Poco::DigestOutputStream& digestStream ) const {

    digestStream << "A";
}

//----------------------------------------------------------------//
void Block::AbstractSerializable_fromJSON ( const Poco::JSON::Object& object ) {
}

//----------------------------------------------------------------//
void Block::AbstractSerializable_toJSON ( Poco::JSON::Object& object ) const {
}


} // namespace Volition
