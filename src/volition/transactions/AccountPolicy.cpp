// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Hash.h>
#include <volition/Serialize.h>
#include <volition/transactions/AccountPolicy.h>

namespace Volition {
namespace Transaction {

//================================================================//
// AccountPolicy
//================================================================//

//----------------------------------------------------------------//
AccountPolicy::AccountPolicy () {
}

//----------------------------------------------------------------//
AccountPolicy::~AccountPolicy () {
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
void AccountPolicy::AbstractHashable_hash ( Poco::DigestOutputStream& digestStream ) const {
    AbstractTransaction::AbstractHashable_hash ( digestStream );

    digestStream << this->mAccountName;
    digestStream << this->mPolicyName;

    Hash::hashOrNull ( digestStream, this->mPolicy.get ());
}

//----------------------------------------------------------------//
void AccountPolicy::AbstractSerializable_fromJSON ( const Poco::JSON::Object& object ) {
    AbstractTransaction::AbstractSerializable_fromJSON ( object );
   
    this->mAccountName              = object.optValue < string >( "accountName", "" );
    this->mPolicyName               = object.optValue < string >( "policyName", "" );
    
    this->mPolicy                   = Serialize::getSerializableFromJSON < Policy >( object, "policy" );
}

//----------------------------------------------------------------//
void AccountPolicy::AbstractSerializable_toJSON ( Poco::JSON::Object& object ) const {
    AbstractTransaction::AbstractSerializable_toJSON ( object );

    object.set ( "accountName",     this->mAccountName.c_str ());
    object.set ( "policyName",      this->mPolicyName.c_str ());
    
    Serialize::setSerializableToJSON ( object, "policy", this->mPolicy.get ());
}

//----------------------------------------------------------------//
void AccountPolicy::AbstractTransaction_apply ( State& state ) const {
}

} // namespace Transaction
} // namespace Volition
