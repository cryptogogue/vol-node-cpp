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
void Signature::digest ( const AbstractHashable& hashable, Poco::Crypto::ECDSADigestEngine& digestEngine ) {

    Poco::DigestOutputStream signatureStream ( digestEngine );
    hashable.hash ( signatureStream );
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
const Poco::DigestEngine::Digest& Signature::sign ( const AbstractHashable& hashable, const Poco::Crypto::ECKey& key, string hashAlgorithm ) {

    this->mHashAlgorithm = hashAlgorithm;

    Poco::Crypto::ECDSADigestEngine signature ( key, this->mHashAlgorithm );
    this->digest ( hashable, signature );
    
    this->mDigest = signature.digest ();
    this->mSignature = signature.signature ();
    return this->mSignature;
}

//----------------------------------------------------------------//
Signature::Signature () {
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
bool Signature::verify ( const AbstractHashable& hashable, const Poco::Crypto::ECKey& key ) const {

    Poco::Crypto::ECDSADigestEngine signature ( key, this->mHashAlgorithm );
    this->digest ( hashable, signature );
    
    return signature.verify ( this->mSignature );
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
void Signature::AbstractSerializable_fromJSON ( const Poco::JSON::Object& object ) {

    string digestString             = object.optValue < string >( "digest", "" );
    string signatureString          = object.optValue < string >( "signature", "" );

    this->mDigest                   = digestString.size () ? Poco::DigestEngine::digestFromHex ( digestString ) : Poco::DigestEngine::Digest ();
    this->mSignature                = signatureString.size () ? Poco::DigestEngine::digestFromHex ( signatureString ) : Poco::DigestEngine::Digest ();
    this->mHashAlgorithm            = object.optValue < string >( "hashAlgorithm", DEFAULT_HASH_ALGORITHM );
}

//----------------------------------------------------------------//
void Signature::AbstractSerializable_toJSON ( Poco::JSON::Object& object ) const {
    
    object.set ( "digest",          Poco::DigestEngine::digestToHex ( this->mDigest ).c_str ());
    object.set ( "signature",       Poco::DigestEngine::digestToHex ( this->mSignature ).c_str ());
    object.set ( "hashAlgorithm",   this->mHashAlgorithm.c_str ());
}

} // namespace Volition
