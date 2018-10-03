// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Policy.h>

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
void Policy::AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) {
}

//----------------------------------------------------------------//
void Policy::AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const {
}

} // namespace Volition
