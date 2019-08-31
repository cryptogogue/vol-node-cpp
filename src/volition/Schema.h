// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SCHEMA_H
#define VOLITION_SCHEMA_H

#include <volition/common.h>
#include <volition/AssetDefinition.h>
#include <volition/AssetMethod.h>
#include <volition/Ledger.h>
#include <volition/SquapFactory.h>

namespace Volition {

//================================================================//
// Schema
//================================================================//
class Schema :
    public AbstractSerializable {
public:

    typedef u64 Index;
    enum {
        NULL_INDEX      = ( u64 )-1,
    };

private:

    friend class Ledger;
    friend class LuaContext;

    typedef SerializableMap < string, AssetDefinition >     Definitions;
    typedef SerializableMap < string, AssetMethod >         Methods;

    string                  mName;
    string                  mMeta;
    Definitions             mDefinitions;
    Methods                 mMethods;
    string                  mLua;

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) {

        serializer.serialize ( "name",              this->mName );
        serializer.serialize ( "definitions",       this->mDefinitions );
        serializer.serialize ( "methods",           this->mMethods );
        serializer.serialize ( "lua",               this->mLua );
        
        serializer.stringFromTree ( "meta", this->mMeta );
    }

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const {

        serializer.serialize ( "name",              this->mName );
        serializer.serialize ( "definitions",       this->mDefinitions );
        serializer.serialize ( "methods",           this->mMethods );
        serializer.serialize ( "lua",               this->mLua );
        
        if ( serializer.isDigest ()) {
        
            serializer.serialize ( "meta", this->mMeta );
        }
        else {
            serializer.stringToTree ( "meta", this->mMeta );
        }
    }
};

} // namespace Volition
#endif
