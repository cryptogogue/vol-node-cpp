// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

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
string Signable::getHashAlgorithm () const {

    return this->mHashAlgorithm;
}

//----------------------------------------------------------------//
const Poco::Crypto::ECKey* Signable::getPublicKey () const {

    return this->mPublicKey.get ();
}

//----------------------------------------------------------------//
const Poco::DigestEngine::Digest& Signable::sign ( const Poco::Crypto::ECKey& key, string hashAlgorithm ) {

    return this->Signable_sign ( key, hashAlgorithm );
}

//----------------------------------------------------------------//
Signable::Signable () {
}

//----------------------------------------------------------------//
Signable::~Signable () {
}

//----------------------------------------------------------------//
const Poco::DigestEngine::Digest& Signable::Signable_sign ( const Poco::Crypto::ECKey& key, string hashAlgorithm ) {

    this->mHashAlgorithm = hashAlgorithm;
    this->mPublicKey = make_unique < Poco::Crypto::ECKey >( key );

    Poco::Crypto::ECDSADigestEngine signature ( *this->mPublicKey, this->mHashAlgorithm );
    Poco::DigestOutputStream signatureStream ( signature );
    this->hash ( signatureStream );
    signatureStream.close ();
    
    this->mDigest = signature.digest ();
    this->mSignature = signature.signature ();
    return this->mSignature;
}

//----------------------------------------------------------------//
bool Signable::Signable_verify ( const Poco::Crypto::ECKey& key, string hashAlgorithm ) const {

    Poco::Crypto::ECDSADigestEngine signature ( key, hashAlgorithm );
    Poco::DigestOutputStream signatureStream ( signature );
    this->hash ( signatureStream );
    signatureStream.close ();
    
    return signature.verify ( this->mSignature );
}

//----------------------------------------------------------------//
string Signable::toHex ( const Poco::DigestEngine::Digest& digest ) {

    return Poco::DigestEngine::digestToHex ( digest );
}

//----------------------------------------------------------------//
bool Signable::verify () const {

    return ( this->mPublicKey && this->Signable_verify ( *this->mPublicKey, this->mHashAlgorithm ));
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
void Signable::AbstractSerializable_fromJSON ( const Poco::JSON::Object& object ) {

    string digestString             = object.optValue < string >( "digest", "" );
    string signatureString          = object.optValue < string >( "signature", "" );
    string keyString                = object.optValue < string >( "key", "" );
    
    stringstream keyStream ( keyString );

    this->mDigest                   = digestString.size () ? Poco::DigestEngine::digestFromHex ( digestString ) : Poco::DigestEngine::Digest ();
    this->mSignature                = signatureString.size () ? Poco::DigestEngine::digestFromHex ( signatureString ) : Poco::DigestEngine::Digest ();
    this->mPublicKey                = keyString.size () ? make_unique < Poco::Crypto::ECKey >( &keyStream ) : NULL;
    this->mHashAlgorithm            = object.optValue < string >( "hashAlgorithm", DEFAULT_HASH_ALGORITHM );
}

//----------------------------------------------------------------//
void Signable::AbstractSerializable_toJSON ( Poco::JSON::Object& object ) const {

    stringstream publicKeyString;
    this->mPublicKey->save ( &publicKeyString );
    
    object.set ( "digest",          Poco::DigestEngine::digestToHex ( this->mDigest ).c_str ());
    object.set ( "signature",       Poco::DigestEngine::digestToHex ( this->mSignature ).c_str ());
    object.set ( "key",             publicKeyString.str ().c_str ());
    object.set ( "hashAlgorithm",   this->mHashAlgorithm.c_str ());
}

} // namespace Volition
