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
    
    string opName = serializer.serializeIn < string >( "op", "" );
    AbstractSquap::OpCode opCode = ( AbstractSquap::OpCode )FNV1a::hash_64 ( opName.c_str ());
    
    unique_ptr < AbstractSquap > squap;
    
    switch ( opCode ) {
        case AbstractSquap::ADD:
        case AbstractSquap::AND:
        case AbstractSquap::EQUAL:
        case AbstractSquap::DIV:
        case AbstractSquap::GREATER:
        case AbstractSquap::GREATER_OR_EQUAL:
        case AbstractSquap::LESS_OR_EQUAL:
        case AbstractSquap::LESS:
        case AbstractSquap::MOD:
        case AbstractSquap::MUL:
        case AbstractSquap::NOT_EQUAL:
        case AbstractSquap::OR:
        case AbstractSquap::SUB:
        case AbstractSquap::XOR:
            squap = make_unique < BinarySquap >();
            break;
        
        case AbstractSquap::CONST:
            squap =  make_unique < ConstSquap >();
            break;
        
        case AbstractSquap::ASSET_TYPE:
        case AbstractSquap::FIELD:
        case AbstractSquap::IN:
            squap =  make_unique < FuncSquap >();
            break;
        
        case AbstractSquap::NOT:
            squap =  make_unique < UnarySquap >();
            break;
    }
    
    if ( squap ) {
        squap->setOpCode ( opCode );
    }
    
    return squap;
}

} // namespace Volition
