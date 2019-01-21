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

    Variant    mValue;
    
    //----------------------------------------------------------------//
    Variant AbstractSquap_evaluate () const override {
        
        return this->mValue;
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        
        serializer.serialize < Variant >( "value", this->mValue );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        assert ( false ); // unsupported
    }
};

} // namespace Volition
#endif
