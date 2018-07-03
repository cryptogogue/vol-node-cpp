// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include "TransferFunds.h"
#include "Serialize.h"

namespace Volition {
namespace Transaction {

//================================================================//
// TransferFunds
//================================================================//

//----------------------------------------------------------------//
TransferFunds::TransferFunds () {
}

//----------------------------------------------------------------//
TransferFunds::~TransferFunds () {
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
void TransferFunds::AbstractHashable_hash ( Poco::DigestOutputStream& digestStream ) const {

    digestStream << this->mAccountName;
    digestStream << this->mAmount;
}

//----------------------------------------------------------------//
void TransferFunds::AbstractSerializable_fromJSON ( const Poco::JSON::Object& object ) {
    AbstractTransaction::AbstractSerializable_fromJSON ( object );
    
    this->mAccountName          = object.optValue < string >( "accountName", "" );
    this->mAmount               = Serialize::getU64FromJSON ( object, "amount", 0 );
}

//----------------------------------------------------------------//
void TransferFunds::AbstractSerializable_toJSON ( Poco::JSON::Object& object ) const {
    AbstractTransaction::AbstractSerializable_toJSON ( object );
    
    object.set ( "accountName", this->mAccountName.c_str ());
    Serialize::setU64ToJSON ( object, "amount", this->mAmount );
}

//----------------------------------------------------------------//
void TransferFunds::AbstractTransaction_apply ( State& state ) const {
}

} // namespace Transaction
} // namespace Volition
