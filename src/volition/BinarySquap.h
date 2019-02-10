// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_BINARYSQUAP_H
#define VOLITION_BINARYSQUAP_H

#include <volition/common.h>
#include <volition/SquapFactory.h>

namespace Volition {

//================================================================//
// BinarySquap
//================================================================//
class BinarySquap :
     public AbstractSquap {
public:

    SerializableSharedPtr < AbstractSquap, SquapFactory >   mLeft;
    SerializableSharedPtr < AbstractSquap, SquapFactory >   mRight;
    
    //----------------------------------------------------------------//
    Variant AbstractSquap_evaluate () const override {
        
        if ( !( this->mLeft && this->mRight )) return Variant ();
        
        const AbstractSquap& lval = *this->mLeft;
        const AbstractSquap& rval = *this->mRight;
        
        switch ( this->mOpCode ) {
            case AND:
                return Variant::booleanAnd ( lval, rval );
                
            case ADD:
                return Variant::add ( lval, rval );
                
            case DIV:
                return Variant::div ( lval, rval );
                
            case EQUAL:
                return Variant::equal ( lval, rval );
                
            case GREATER:
                return Variant::greater ( lval, rval );
                
            case GREATER_OR_EQUAL:
                return Variant::greaterOrEqual ( lval, rval );
            
            case LESS:
                return Variant::less ( lval, rval );
            
            case LESS_OR_EQUAL:
                return Variant::lessOrEqual ( lval, rval );
                
            case MOD:
                return Variant::mod ( lval, rval );
                
            case MUL:
                return Variant::mul ( lval, rval );
                
            case NOT_EQUAL:
                return Variant::notEqual ( lval, rval );
                
            case OR:
                return Variant::booleanOr ( lval, rval );
                
            case SUB:
                return Variant::sub ( lval, rval );
                
            case XOR:
                return Variant::booleanXor ( lval, rval );
            
            default:
                return Variant ();
        };
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
    
        serializer.serialize ( "left",          this->mLeft );
        serializer.serialize ( "right",         this->mRight );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractSquap::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "left",          this->mLeft );
        serializer.serialize ( "right",         this->mRight );
    }
};

} // namespace Volition
#endif
