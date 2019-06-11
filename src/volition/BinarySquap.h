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
    AssetFieldValue AbstractSquap_evaluate ( const SquapEvaluationContext& context ) const override {
        
        if ( !( this->mLeft && this->mRight )) return AssetFieldValue ();
        
        AssetFieldValue lval = this->mLeft->evaluate ( context );
        AssetFieldValue rval = this->mRight->evaluate ( context );
        
        switch ( this->mOpCode ) {
            case AND:
                return AssetFieldValue::booleanAnd ( lval, rval );
                
            case ADD:
                return AssetFieldValue::add ( lval, rval );
                
            case DIV:
                return AssetFieldValue::div ( lval, rval );
                
            case EQUAL:
                return AssetFieldValue::equal ( lval, rval );
                
            case GREATER:
                return AssetFieldValue::greater ( lval, rval );
                
            case GREATER_OR_EQUAL:
                return AssetFieldValue::greaterOrEqual ( lval, rval );
            
            case KEYWORD:
                return false; // TODO: squap
            
            case LESS:
                return AssetFieldValue::less ( lval, rval );
            
            case LESS_OR_EQUAL:
                return AssetFieldValue::lessOrEqual ( lval, rval );
                
            case MOD:
                return AssetFieldValue::mod ( lval, rval );
                
            case MUL:
                return AssetFieldValue::mul ( lval, rval );
                
            case NOT_EQUAL:
                return AssetFieldValue::notEqual ( lval, rval );
                
            case OR:
                return AssetFieldValue::booleanOr ( lval, rval );
                
            case SUB:
                return AssetFieldValue::sub ( lval, rval );
                
            case XOR:
                return AssetFieldValue::booleanXor ( lval, rval );
            
            default:
                return AssetFieldValue ();
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
