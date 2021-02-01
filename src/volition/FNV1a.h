// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_FNV1A_H
#define VOLITION_FNV1A_H

#include <volition/common.h>

namespace Volition {
namespace FNV1a {

//================================================================//
// FNV1a
//================================================================//

constexpr uint32_t  FNV1A_VAL_32        = 0x811c9dc5;
constexpr uint32_t  FNV1A_PRIME_32      = 0x1000193;
constexpr uint64_t  FNV1A_VAL_64        = 0xcbf29ce484222325;
constexpr uint64_t  FNV1A_PRIME_64      = 0x100000001b3;

//----------------------------------------------------------------//
constexpr uint32_t const_hash_32 ( const char* str, uint32_t hash = FNV1A_VAL_32 ) {
    return *str ? const_hash_32 ( &str [ 1 ], ( hash ^ ( uint32_t )*str ) * FNV1A_PRIME_32 ) : hash;
}

//----------------------------------------------------------------//
constexpr uint64_t const_hash_64 ( const char* str, uint64_t hash = FNV1A_VAL_64 ) {
    return *str ? const_hash_64 ( &str [ 1 ], ( hash ^ ( uint64_t )*str ) * FNV1A_PRIME_64 ) : hash;
}

//----------------------------------------------------------------//
inline uint32_t hash_32 ( const char* str ) noexcept {

    uint32_t hash = FNV1A_VAL_32;
    for ( ; *str; ++str ) {
        hash = ( hash ^ ( uint32_t )*str ) * FNV1A_PRIME_32;
    }
    return hash;
}

//----------------------------------------------------------------//
inline uint64_t hash_64 ( const char* str ) noexcept {

    uint64_t hash = FNV1A_VAL_64;
    for ( ; *str; ++str ) {
        hash = ( hash ^ ( uint64_t )*str ) * FNV1A_PRIME_64;
    }
    return hash;
}

} // namespace FNV1a
} // namespace Volition
#endif
