// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/BinarySquap.h>
#include <volition/ConstSquap.h>
#include <volition/FuncSquap.h>
#include <volition/SquapFactory.h>
#include <volition/UnarySquap.h>

namespace Volition {

//================================================================//
// SquapFactory
//================================================================//

//----------------------------------------------------------------//
unique_ptr < AbstractSquap > SquapFactory::SerializablePtrFactory_make ( const AbstractSerializerFrom& serializer ) {
    
    string opname = serializer.serializeIn < string >( "op", "" );
    
    switch ( FNV1a::hash_64 ( opname.c_str ())) {
        case FNV1a::const_hash_64 ( "ADD" ):
        case FNV1a::const_hash_64 ( "AND" ):
        case FNV1a::const_hash_64 ( "EQUAL" ):
        case FNV1a::const_hash_64 ( "DIV" ):
        case FNV1a::const_hash_64 ( "GREATER" ):
        case FNV1a::const_hash_64 ( "GREATER_OR_EQUAL" ):
        case FNV1a::const_hash_64 ( "LESS_OR_EQUAL" ):
        case FNV1a::const_hash_64 ( "LESS" ):
        case FNV1a::const_hash_64 ( "MOD" ):
        case FNV1a::const_hash_64 ( "MUL" ):
        case FNV1a::const_hash_64 ( "NOT_EQUAL" ):
        case FNV1a::const_hash_64 ( "OR" ):
        case FNV1a::const_hash_64 ( "SUB" ):
        case FNV1a::const_hash_64 ( "XOR" ):
            return make_unique < BinarySquap >();
        
        case FNV1a::const_hash_64 ( "CONST" ):
            return make_unique < ConstSquap >();
        
        case FNV1a::const_hash_64 ( "FIELD" ):
        case FNV1a::const_hash_64 ( "IS" ):
        case FNV1a::const_hash_64 ( "IN" ):
            return make_unique < FuncSquap >();
        
        case FNV1a::const_hash_64 ( "NOT" ):
            return make_unique < UnarySquap >();
    }
    
    return NULL;
}

} // namespace Volition
