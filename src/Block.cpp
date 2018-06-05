//
//  main.cpp
// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include "Block.h"

namespace Volition {

//================================================================//
// Block
//================================================================//

//----------------------------------------------------------------//
Block::Block () {
}

//----------------------------------------------------------------//
Block::~Block () {
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
void Block::AbstractHashable_hash ( Poco::DigestOutputStream& digestStream ) const {

    digestStream << "A";
}

//----------------------------------------------------------------//
void Block::AbstractSerializable_fromJSON ( const Poco::JSON::Object& object ) {
}

//----------------------------------------------------------------//
void Block::AbstractSerializable_toJSON ( Poco::JSON::Object& object ) const {
}


} // namespace Volition
