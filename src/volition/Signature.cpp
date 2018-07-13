// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Signature.h>

namespace Volition {

//================================================================//
// Signature
//================================================================//

//----------------------------------------------------------------//
void Signature::digest ( string str, Poco::Crypto::ECDSADigestEngine& digestEngine ) {

    Poco::DigestOutputStream signatureStream ( digestEngine );
    signatureStream << str;
    signatureStream.close ();
}

//----------------------------------------------------------------//
void Signature::digest ( AbstractSerializable& serializable, Poco::Crypto::ECDSADigestEngine& digestEngine ) {

    Poco::DigestOutputStream signatureStream ( digestEngine );
    DigestSerializer serializer;
    serializer.hash ( serializable, signatureStream );
    signatureStream.close ();
}

//----------------------------------------------------------------//
const Poco::DigestEngine::Digest& Signature::getDigest () const {

    return this->mDigest;
}

//----------------------------------------------------------------//
const Poco::DigestEngine::Digest& Signature::getSignature () const {

    return this->mSignature;
}

//----------------------------------------------------------------//
string Signature::getHashAlgorithm () const {

    return this->mHashAlgorithm;
}

//----------------------------------------------------------------//
const Poco::DigestEngine::Digest& Signature::sign ( string str, const Poco::Crypto::ECKey& key, string hashAlgorithm ) {

    this->mHashAlgorithm = hashAlgorithm;

    Poco::Crypto::ECDSADigestEngine signature ( key, this->mHashAlgorithm );
    this->digest ( str, signature );
    
    this->mDigest = signature.digest ();
    this->mSignature = signature.signature ();
    return this->mSignature;
}

//----------------------------------------------------------------//
const Poco::DigestEngine::Digest& Signature::sign ( AbstractSerializable& serializable, const Poco::Crypto::ECKey& key, string hashAlgorithm ) {

    this->mHashAlgorithm = hashAlgorithm;

    Poco::Crypto::ECDSADigestEngine signature ( key, this->mHashAlgorithm );
    this->digest ( serializable, signature );
    
    this->mDigest = signature.digest ();
    this->mSignature = signature.signature ();
    return this->mSignature;
}

//----------------------------------------------------------------//
Signature::Signature () :
    mHashAlgorithm ( DEFAULT_HASH_ALGORITHM) {
}

//----------------------------------------------------------------//
Signature::~Signature () {
}

//----------------------------------------------------------------//
string Signature::toHex ( const Poco::DigestEngine::Digest& digest ) {

    return Poco::DigestEngine::digestToHex ( digest );
}

//----------------------------------------------------------------//
bool Signature::verify ( string str, const Poco::Crypto::ECKey& key ) const {

    Poco::Crypto::ECDSADigestEngine signature ( key, this->mHashAlgorithm );
    this->digest ( str, signature );
    
    return signature.verify ( this->mSignature );
}

//----------------------------------------------------------------//
bool Signature::verify ( AbstractSerializable& serializable, const Poco::Crypto::ECKey& key ) const {

    Poco::Crypto::ECDSADigestEngine signature ( key, this->mHashAlgorithm );
    this->digest ( serializable, signature );

    return signature.verify ( this->mSignature );
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
void Signature::AbstractSerializable_serialize ( AbstractSerializer& serializer ) {

    serializer.serialize ( "hashAlgorithm",     this->mHashAlgorithm );
    serializer.serialize ( "digest",            this->mDigest );
    serializer.serialize ( "signature",         this->mSignature );
}

} // namespace Volition
