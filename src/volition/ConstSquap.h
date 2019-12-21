// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_CONSTSQUAP_H
#define VOLITION_CONSTSQUAP_H

#include <volition/common.h>
#include <volition/SquapFactory.h>

namespace Volition {

//================================================================//
// ConstSquap
//================================================================//
class ConstSquap :
     public AbstractSquap {
public:

    AssetFieldVariant     mConst; // TODO: this is a mistake here; should just be a variant
    
    //----------------------------------------------------------------//
    AssetFieldVariant AbstractSquap_evaluate ( const SquapEvaluationContext& context ) const override {
        UNUSED ( context );
    
        return this->mConst;
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        
        string typeStr;
        serializer.serialize ( "type", typeStr );
        AssetFieldVariant::Type type = ( AssetFieldVariant::Type )FNV1a::hash_64 ( typeStr.c_str ());
        
        this->mConst.serializeValue ( serializer, type, "const" );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractSquap::AbstractSerializable_serializeTo ( serializer );
        
        this->mConst.serializeValue ( serializer, "const" );
    }
};

} // namespace Volition
#endif
