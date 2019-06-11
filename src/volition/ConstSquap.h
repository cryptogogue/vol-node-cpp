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

    AssetFieldValue     mConst;
    
    //----------------------------------------------------------------//
    AssetFieldValue AbstractSquap_evaluate ( const SquapEvaluationContext& context ) const override {
        
        return this->mConst;
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        
        serializer.serialize < AssetFieldValue >( "const", this->mConst );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractSquap::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize < AssetFieldValue >( "const", this->mConst );
    }
};

} // namespace Volition
#endif
