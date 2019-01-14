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
    SquapVal AbstractSquap_evaluate () const override {
        
        assert ( this->mOpCode == NOT );
        if ( this->mOperand ) return SquapVal::booleanNot ( !this->mOperand );
        return SquapVal ();
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
    
        serializer.serialize ( "operand",       this->mOperand );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        assert ( false ); // unsupported
    }
};

} // namespace Volition
#endif
