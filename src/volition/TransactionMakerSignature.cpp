// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <Serialize.h>
#include <TransactionMakerSignature.h>

namespace Volition {

//================================================================//
// TransactionMakerSignature
//================================================================//

//----------------------------------------------------------------//
TransactionMakerSignature::TransactionMakerSignature () :
    mGratuity ( 0 ),
    mNonce ( 0 ) {
}

//----------------------------------------------------------------//
TransactionMakerSignature::~TransactionMakerSignature () {
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
void TransactionMakerSignature::AbstractHashable_hash ( Poco::DigestOutputStream& digestStream ) const {

    digestStream << this->mGratuity;
    digestStream << this->mKeyName;
    digestStream << this->mNonce;
}

//----------------------------------------------------------------//
void TransactionMakerSignature::AbstractSerializable_fromJSON ( const Poco::JSON::Object& object ) {
    Signature::AbstractSerializable_fromJSON ( object );

    this->mKeyName      = object.optValue < string >( "keyName", "" );
    
    this->mGratuity     = Serialize::getU64FromJSON ( object, "gratuity", 0 );
    this->mNonce        = Serialize::getU64FromJSON ( object, "nonce", 0 );
}

//----------------------------------------------------------------//
void TransactionMakerSignature::AbstractSerializable_toJSON ( Poco::JSON::Object& object ) const {
    Signature::AbstractSerializable_toJSON ( object );

    object.set ( "keyName",       this->mKeyName.c_str ());
    
    Serialize::setU64ToJSON ( object, "gratuity",   this->mGratuity );
    Serialize::setU64ToJSON ( object, "nonce",     this->mNonce );
}

} // namespace Volition
