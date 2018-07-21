// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Signature.h>

namespace Volition {

//================================================================//
// Signature
//================================================================//

//----------------------------------------------------------------//
const Digest& Signature::getDigest () const {

    return this->mDigest;
}

//----------------------------------------------------------------//
const Digest& Signature::getSignature () const {

    return this->mSignature;
}

//----------------------------------------------------------------//
string Signature::getHashAlgorithm () const {

    return this->mHashAlgorithm;
}

//----------------------------------------------------------------//
Signature::Signature () :
    mHashAlgorithm ( DEFAULT_HASH_ALGORITHM) {
}

//----------------------------------------------------------------//
Signature::Signature ( Digest digest, Digest signature, string hashAlgorithm ) :
    mDigest ( digest ),
    mSignature ( signature ),
    mHashAlgorithm ( hashAlgorithm ) {
}

//----------------------------------------------------------------//
Signature::~Signature () {
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
