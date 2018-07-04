// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Hash.h>
#include <volition/Serialize.h>
#include <volition/transactions/KeyPolicy.h>

namespace Volition {
namespace Transaction {

//================================================================//
// KeyPolicy
//================================================================//

//----------------------------------------------------------------//
KeyPolicy::KeyPolicy () {
}

//----------------------------------------------------------------//
KeyPolicy::~KeyPolicy () {
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
void KeyPolicy::AbstractHashable_hash ( Poco::DigestOutputStream& digestStream ) const {
    AbstractTransaction::AbstractHashable_hash ( digestStream );

    digestStream << this->mKeyName;
    digestStream << this->mPolicyName;

    Hash::hashOrNull ( digestStream, this->mKey.get ());
    Hash::hashOrNull ( digestStream, this->mPolicy.get ());
}

//----------------------------------------------------------------//
void KeyPolicy::AbstractSerializable_fromJSON ( const Poco::JSON::Object& object ) {
    AbstractTransaction::AbstractSerializable_fromJSON ( object );
   
    this->mKeyName                  = object.optValue < string >( "keyName", "" );
    this->mPolicyName               = object.optValue < string >( "policyName", "" );
    
    this->mKey                      = Serialize::getSerializableFromJSON < Poco::Crypto::ECKey >( object, "key" );
    this->mPolicy                   = Serialize::getSerializableFromJSON < Policy >( object, "policy" );
}

//----------------------------------------------------------------//
void KeyPolicy::AbstractSerializable_toJSON ( Poco::JSON::Object& object ) const {
    AbstractTransaction::AbstractSerializable_toJSON ( object );

    object.set ( "keyName",         this->mKeyName.c_str ());
    object.set ( "policyName",      this->mPolicyName.c_str ());

    Serialize::setSerializableToJSON ( object, "key", this->mKey.get ());
    Serialize::setSerializableToJSON ( object, "policy", this->mPolicy.get ());
}

//----------------------------------------------------------------//
void KeyPolicy::AbstractTransaction_apply ( State& state ) const {
}

} // namespace Transaction
} // namespace Volition
