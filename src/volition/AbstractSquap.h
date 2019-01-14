// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_ABSTRACTSQUAP_H
#define VOLITION_ABSTRACTSQUAP_H

#include <volition/common.h>
#include <volition/serialization/Serialization.h>
#include <volition/SquapVal.h>

namespace Volition {

// Schema QUAlifier oPerator

//================================================================//
// AbstractSquap
//================================================================//
class AbstractSquap :
     public AbstractSerializable {
public:

    enum OpCode {
        ADD,
        AND,
        CONST,
        EQUAL,
        DIV,
        FIELD,
        GREATER,
        GREATER_OR_EQUAL,
        IN,
        IS,
        LESS,
        LESS_OR_EQUAL,
        MOD,
        MUL,
        NOT,
        NOT_EQUAL,
        OR,
        SUB,
        XOR,
    };

    OpCode      mOpCode;
    
    //----------------------------------------------------------------//
    virtual SquapVal        AbstractSquap_evaluate      () const = 0;
    
    //----------------------------------------------------------------//
    operator SquapVal () const {
    
        return this->evaluate ();
    }
    
    //----------------------------------------------------------------//
    SquapVal evaluate () const {
        return this->AbstractSquap_evaluate ();
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
    
        u64 opCode;
        serializer.serialize ( "opCode",       opCode );
        this->mOpCode = ( OpCode )opCode;
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
    
        serializer.serialize ( "opCode",       ( u64 )this->mOpCode );
    }
};

} // namespace Volition
#endif
