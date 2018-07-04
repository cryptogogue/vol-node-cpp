// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Hash.h>

namespace Volition {
namespace Hash {

//================================================================//
// Hash
//================================================================//

//----------------------------------------------------------------//
void hashOrNull ( Poco::DigestOutputStream& digestStream, const AbstractHashable* hashable ) {
    if ( hashable ) {
        hashable->hash ( digestStream );
    }
}

//----------------------------------------------------------------//
void hashOrNull ( Poco::DigestOutputStream& digestStream, const Poco::Crypto::ECKey* hashable ) {
    if ( hashable ) {
        stringstream keyString;
        hashable->save ( &keyString );
        digestStream << keyString.str ();
    }
}

} // namespace Hash
} // namespace Volition
