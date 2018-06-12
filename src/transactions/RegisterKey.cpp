// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include "RegisterKey.h"

namespace Volition {
namespace Transaction {

//================================================================//
// RegisterKey
//================================================================//

//----------------------------------------------------------------//
RegisterKey::RegisterKey () {
}

//----------------------------------------------------------------//
RegisterKey::~RegisterKey () {
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
void RegisterKey::AbstractHashable_hash ( Poco::DigestOutputStream& digestStream ) const {

    stringstream publicKeyString;
    this->mPublicKey->save ( &publicKeyString );

    digestStream << publicKeyString.str ();
    digestStream << this->mKeyName;
}

//----------------------------------------------------------------//
void RegisterKey::AbstractSerializable_fromJSON ( const Poco::JSON::Object& object ) {
    AbstractTransaction::AbstractSerializable_fromJSON ( object );
    
    string keyString                = object.optValue < string >( "publicKey", "" );
    
    stringstream keyStream ( keyString );
   
    this->mPublicKey                = keyString.size () ? make_unique < Poco::Crypto::ECKey >( &keyStream ) : NULL;
    this->mKeyName                  = object.optValue < string >( "keyName", "" );
}

//----------------------------------------------------------------//
void RegisterKey::AbstractSerializable_toJSON ( Poco::JSON::Object& object ) const {
    AbstractTransaction::AbstractSerializable_toJSON ( object );

    stringstream publicKeyString;
    this->mPublicKey->save ( &publicKeyString );

    object.set ( "publicKey",   publicKeyString.str ().c_str ());
    object.set ( "keyName",     this->mKeyName.c_str ());
}

//----------------------------------------------------------------//
void RegisterKey::AbstractTransaction_apply ( State& state ) const {
}

} // namespace Transaction
} // namespace Volition
