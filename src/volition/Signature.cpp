// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Signature.h>

namespace Volition {

//================================================================//
// Signature
//================================================================//

//----------------------------------------------------------------//
const Digest& Signature::getSignature () const {

    return *this;
}

//----------------------------------------------------------------//
string Signature::getHashAlgorithm () const {

    return this->mHashAlgorithm;
}

//----------------------------------------------------------------//
Signature::Signature () :
    mHashAlgorithm ( Digest::DEFAULT_HASH_ALGORITHM) {
}

//----------------------------------------------------------------//
Signature::Signature ( Digest signature, string hashAlgorithm ) :
    Digest ( signature ),
    mHashAlgorithm ( hashAlgorithm ) {
}

//----------------------------------------------------------------//
Signature::~Signature () {
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
void Signature::AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) {

    string hex;

    serializer.serialize ( "hashAlgorithm",     this->mHashAlgorithm );
    serializer.serialize ( "signature",         hex );
    
    this->fromString ( hex );
}

//----------------------------------------------------------------//
void Signature::AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const {

    serializer.serialize ( "hashAlgorithm",     this->mHashAlgorithm );
    serializer.serialize ( "signature",         this->toHex ());
}

} // namespace Volition
