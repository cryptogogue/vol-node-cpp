// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_UNARYSQUAP_H
#define VOLITION_UNARYSQUAP_H

#include <volition/common.h>
#include <volition/SquapFactory.h>

namespace Volition {

//================================================================//
// UnarySquap
//================================================================//
class UnarySquap :
     public AbstractSquap {
public:

    SerializableSharedPtr < AbstractSquap, SquapFactory >   mOperand;
    
    //----------------------------------------------------------------//
    AssetFieldValue AbstractSquap_evaluate ( const SquapEvaluationContext& context ) const override {
        
        if ( !this->mOperand ) return AssetFieldValue ();
        
        AssetFieldValue val = this->mOperand->evaluate ( context );
        
        switch ( this->mOpCode ) {
            
            case LENGTH:
                return AssetFieldValue::length ( val );
                
            case NOT:
                return AssetFieldValue::booleanNot ( val );
            
            default:
                return AssetFieldValue ();
        };
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
    
        serializer.serialize ( "operand",       this->mOperand );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractSquap::AbstractSerializable_serializeTo ( serializer );
     
        serializer.serialize ( "operand",       this->mOperand );
    }
};

} // namespace Volition
#endif
