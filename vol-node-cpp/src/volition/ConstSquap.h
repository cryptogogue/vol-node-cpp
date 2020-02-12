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
     public AbstractSquap,
     public AssetFieldValue {
public:
    
    //----------------------------------------------------------------//
    AssetFieldValue AbstractSquap_evaluate ( const SquapEvaluationContext& context ) const override {
        UNUSED ( context );
        return *this;
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AssetFieldValue::AbstractSerializable_serializeFrom ( serializer );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractSquap::AbstractSerializable_serializeTo ( serializer );
        AssetFieldValue::AbstractSerializable_serializeTo ( serializer );
    }
};

} // namespace Volition
#endif
