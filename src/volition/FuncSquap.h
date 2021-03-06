// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_FUNCSQUAP_H
#define VOLITION_FUNCSQUAP_H

#include <volition/common.h>
#include <volition/SquapFactory.h>

namespace Volition {

//================================================================//
// FuncSquap
//================================================================//
class FuncSquap :
     public AbstractSquap {
public:

    string  mFuncName;
    SerializableVector < SerializableSharedPtr < AbstractSquap, SquapFactory >> mArgs;
    
    //----------------------------------------------------------------//
    AssetFieldValue AbstractSquap_evaluate ( const SquapEvaluationContext& context ) const override {
        UNUSED ( context );
        
        return AssetFieldValue ();
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
    
        serializer.serialize ( "func",      this->mFuncName );
        serializer.serialize ( "args",      this->mArgs );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractSquap::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "func",      this->mFuncName );
        serializer.serialize ( "args",      this->mArgs );
    }
};

} // namespace Volition
#endif
