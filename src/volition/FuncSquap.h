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

    SerializableVector < SerializableSharedPtr < AbstractSquap, SquapFactory >>     mArgs;
    
    //----------------------------------------------------------------//
    Variant AbstractSquap_evaluate () const override {
        
        return Variant ();
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
    
        serializer.serialize ( "args",      this->mArgs );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        assert ( false ); // unsupported
    }
};

} // namespace Volition
#endif
