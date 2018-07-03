//
//  main.cpp
// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include "Policy.h"

namespace Volition {

//================================================================//
// Policy
//================================================================//

//----------------------------------------------------------------//
Policy::Policy () {
}

//----------------------------------------------------------------//
Policy::~Policy () {
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
void Policy::AbstractHashable_hash ( Poco::DigestOutputStream& digestStream ) const {
}

//----------------------------------------------------------------//
void Policy::AbstractSerializable_fromJSON ( const Poco::JSON::Object& object ) {
}

//----------------------------------------------------------------//
void Policy::AbstractSerializable_toJSON ( Poco::JSON::Object& object ) const {
}

} // namespace Volition
