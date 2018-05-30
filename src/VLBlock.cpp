//
//  main.cpp
//  consensus
//
//  Created by Patrick Meehan on 4/27/18.
//  Copyright © 2018 Patrick Meehan. All rights reserved.
//

#include "VLBlock.h"

#include <Poco/DigestEngine.h>
#include <Poco/DigestStream.h>
#include <Poco/MD5Engine.h>
#include <Poco/Crypto/DigestEngine.h>
#include <Poco/Crypto/ECKey.h>
#include <Poco/Crypto/ECKeyImpl.h>
#include <Poco/Crypto/ECDSADigestEngine.h>

//================================================================//
// VLBlock
//================================================================//

//----------------------------------------------------------------//
void VLBlock::Sign () {

    Poco::Crypto::DigestEngine sha256 ( "SHA256" );
    Poco::DigestOutputStream digestStream ( sha256 );
    //digestStream << this->mBlockID;
    //digestStream << this->mPrevBlockHash;
    digestStream << "A";
    digestStream.close ();
    
    this->mBlockHash = Poco::DigestEngine::digestToHex ( sha256.digest ());
    printf ( "HASH: %s\n", this->mBlockHash.c_str ());
    
    Poco::Crypto::ECKey key ( "secp256k1" );
    Poco::Crypto::ECDSADigestEngine signature ( key, "SHA256" );
    Poco::DigestOutputStream signatureStream ( signature );
    signatureStream << "A";
    signatureStream.close ();
    
    string hash = Poco::DigestEngine::digestToHex ( signature.digest ());
    printf ( "HASH: %s\n", hash.c_str ());
    
    string sig = Poco::DigestEngine::digestToHex ( signature.signature ());
    printf ( "SIG: %s\n", sig.c_str ());
}

//----------------------------------------------------------------//
VLBlock::VLBlock () :
    mBlockID ( 0 ) {
}

//----------------------------------------------------------------//
VLBlock::~VLBlock () {
}

