// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include "RegisterMiner.h"

namespace Volition {
namespace Transaction {

//================================================================//
// RegisterMiner
//================================================================//

//----------------------------------------------------------------//
RegisterMiner::RegisterMiner () {
}

//----------------------------------------------------------------//
RegisterMiner::~RegisterMiner () {
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
void RegisterMiner::AbstractHashable_hash ( Poco::DigestOutputStream& digestStream ) const {
}

//----------------------------------------------------------------//
void RegisterMiner::AbstractSerializable_fromJSON ( const Poco::JSON::Object& object ) {

    AbstractTransaction::AbstractSerializable_fromJSON ( object );
    string foo  = object.getValue < string >( "foo" );
    int bar     = object.getValue < int >( "bar" );
    
    printf ( "%s: %s %d\n", RegisterMiner::TYPE_STRING, foo.c_str (), bar );
}

//----------------------------------------------------------------//
void RegisterMiner::AbstractSerializable_toJSON ( Poco::JSON::Object& object ) const {
}

} // namespace Transaction
} // namespace Volition
