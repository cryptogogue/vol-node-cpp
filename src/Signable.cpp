//
//  main.cpp
//  consensus
//
//  Created by Patrick Meehan on 4/27/18.
//  Copyright © 2018 Patrick Meehan. All rights reserved.
//

#include "Signable.h"

namespace Volition {

//================================================================//
// Signable
//================================================================//

//----------------------------------------------------------------//
const Poco::DigestEngine::Digest& Signable::getDigest () const {

    return this->mDigest;
}

//----------------------------------------------------------------//
const Poco::DigestEngine::Digest& Signable::getSignature () const {

    return this->mSignature;
}

//----------------------------------------------------------------//
const Poco::DigestEngine::Digest& Signable::sign ( const Poco::Crypto::ECKey& key, string hashAlgorithm ) {

    Poco::Crypto::ECDSADigestEngine signature ( key, "SHA256" );
    Poco::DigestOutputStream signatureStream ( signature );
    this->hash ( signatureStream );
    signatureStream.close ();
    
    this->mDigest = signature.digest ();
    this->mSignature = signature.signature ();
    return this->mSignature;
}

//----------------------------------------------------------------//
Signable::Signable () {
}

//----------------------------------------------------------------//
Signable::~Signable () {
}

//----------------------------------------------------------------//
string Signable::toHex ( const Poco::DigestEngine::Digest& digest ) {

    return Poco::DigestEngine::digestToHex ( digest );
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//

} // namespace Volition
