// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_HASH_H
#define VOLITION_HASH_H

#include <common.h>
#include <AbstractHashable.h>

namespace Volition {
namespace Hash {

//================================================================//
// Hash
//================================================================//
    
//----------------------------------------------------------------//
void            hashOrNull          ( Poco::DigestOutputStream& digestStream, const AbstractHashable* hashable );
void            hashOrNull          ( Poco::DigestOutputStream& digestStream, const Poco::Crypto::ECKey* hashable );

} // namespace Hash
} // namespace Volition
#endif
