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
    SquapVal AbstractSquap_evaluate () const override {
        
        if ( !( this->mLeft && this->mRight )) return SquapVal ();
        
        const AbstractSquap& lval = *this->mLeft;
        const AbstractSquap& rval = *this->mRight;
        
        switch ( this->mOpCode ) {
            case AND:
                return SquapVal::booleanAnd ( lval, rval );
                
            case ADD:
                return SquapVal::add ( lval, rval );
                
            case DIV:
                return SquapVal::div ( lval, rval );
                
            case EQUAL:
                return SquapVal::equal ( lval, rval );
                
            case GREATER:
                return SquapVal::greater ( lval, rval );
                
            case GREATER_OR_EQUAL:
                return SquapVal::greaterOrEqual ( lval, rval );
            
            case LESS:
                return SquapVal::less ( lval, rval );
            
            case LESS_OR_EQUAL:
                return SquapVal::lessOrEqual ( lval, rval );
                
            case MOD:
                return SquapVal::mod ( lval, rval );
                
            case MUL:
                return SquapVal::mul ( lval, rval );
                
            case NOT_EQUAL:
                return SquapVal::notEqual ( lval, rval );
                
            case OR:
                return SquapVal::booleanOr ( lval, rval );
                
            case SUB:
                return SquapVal::sub ( lval, rval );
                
            case XOR:
                return SquapVal::booleanXor ( lval, rval );
            
            default:
                return SquapVal ();
        };
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
    
        serializer.serialize ( "left",          this->mLeft );
        serializer.serialize ( "right",         this->mRight );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        assert ( false ); // unsupported
    }
};

} // namespace Volition
#endif
