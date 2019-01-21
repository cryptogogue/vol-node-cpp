// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_ABSTRACTSQUAP_H
#define VOLITION_ABSTRACTSQUAP_H

#include <volition/common.h>
#include <volition/serialization/Serialization.h>

namespace Volition {

// Schema QUAlifier oPerator

//================================================================//
// AbstractSquap
//================================================================//
class AbstractSquap :
     public AbstractSerializable {
public:

    enum OpCode : u64 {
        ADD                 = FNV1a::const_hash_64 ( "ADD" ),
        AND                 = FNV1a::const_hash_64 ( "AND" ),
        CONST               = FNV1a::const_hash_64 ( "CONST" ),
        EQUAL               = FNV1a::const_hash_64 ( "EQUAL" ),
        DIV                 = FNV1a::const_hash_64 ( "DIV" ),
        FIELD               = FNV1a::const_hash_64 ( "FIELD" ),
        GREATER             = FNV1a::const_hash_64 ( "GREATER" ),
        GREATER_OR_EQUAL    = FNV1a::const_hash_64 ( "GREATER_OR_EQUAL" ),
        IN                  = FNV1a::const_hash_64 ( "IN" ),
        IS_ASSET            = FNV1a::const_hash_64 ( "IS_ASSET" ),
        LESS                = FNV1a::const_hash_64 ( "LESS" ),
        LESS_OR_EQUAL       = FNV1a::const_hash_64 ( "LESS_OR_EQUAL" ),
        MOD                 = FNV1a::const_hash_64 ( "MOD" ),
        MUL                 = FNV1a::const_hash_64 ( "MUL" ),
        NOT                 = FNV1a::const_hash_64 ( "NOT" ),
        NOT_EQUAL           = FNV1a::const_hash_64 ( "NOT_EQUAL" ),
        OR                  = FNV1a::const_hash_64 ( "OR" ),
        SUB                 = FNV1a::const_hash_64 ( "SUB" ),
        XOR                 = FNV1a::const_hash_64 ( "XOR" ),
    };

    OpCode      mOpCode;
    
    //----------------------------------------------------------------//
    virtual Variant     AbstractSquap_evaluate      () const = 0;
    
    //----------------------------------------------------------------//
    operator Variant () const {
    
        return this->evaluate ();
    }
    
    //----------------------------------------------------------------//
    Variant evaluate () const {
        return this->AbstractSquap_evaluate ();
    }

    //----------------------------------------------------------------//
    void setOpCode ( OpCode opCode ) {
        this->mOpCode = opCode;
    }
};

} // namespace Volition
#endif
