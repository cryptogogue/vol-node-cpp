// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include "TransferFunds.h"

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

    digestStream << this->mURL;
}

//----------------------------------------------------------------//
void TransferFunds::AbstractSerializable_fromJSON ( const Poco::JSON::Object& object ) {

    AbstractTransaction::AbstractSerializable_fromJSON ( object );
    this->mURL = object.optValue < string >( "url", "" );
}

//----------------------------------------------------------------//
void TransferFunds::AbstractSerializable_toJSON ( Poco::JSON::Object& object ) const {

    AbstractTransaction::AbstractSerializable_toJSON ( object );
    object.set ( "url", this->mURL.c_str ());
}

} // namespace Transaction
} // namespace Volition
