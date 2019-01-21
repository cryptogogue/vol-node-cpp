// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SCHEMA_H
#define VOLITION_SCHEMA_H

#include <volition/common.h>
#include <volition/Ledger.h>
#include <volition/SquapFactory.h>

namespace Volition {

//================================================================//
// SchemaAssetTemplateField
//================================================================//
class SchemaAssetTemplateField :
     public AbstractSerializable {
public:

    string              mType;
    bool                mArray;
    bool                mMutable;
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
    
        serializer.serialize ( "type",              this->mType );
        serializer.serialize ( "array",             this->mArray );
        serializer.serialize ( "mutable",           this->mMutable );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        assert ( false ); // unsupported
    }
};

//================================================================//
// SchemaAssetTemplate
//================================================================//
class SchemaAssetTemplate :
     public AbstractSerializable {
public:

    string                                                  mExtends;
    SerializableMap < string, SchemaAssetTemplateField >    mFields;
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
    
        serializer.serialize ( "extends",           this->mExtends );
        serializer.serialize ( "fields",            this->mFields );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        assert ( false ); // unsupported
    }
};

//================================================================//
// SchemaAsset
//================================================================//
class SchemaAsset :
     public AbstractSerializable {
public:

    string                                                      mImplements;
    SerializableMap < string, SerializableVector < Variant >>   mFields;
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
    
        serializer.serialize ( "implements",        this->mImplements );
        serializer.serialize ( "fields",            this->mFields );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        assert ( false ); // unsupported
    }
};

//================================================================//
// SchemaMethod
//================================================================//
class SchemaMethod :
     public AbstractSerializable {
public:

    string                                                                              mDescription;       // friendly description for the rule.
    u64                                                                                 mWeight;
    u64                                                                                 mMaturity;
    SerializableMap < string, SerializableSharedPtr < AbstractSquap, SquapFactory >>    mArgs;        // list of qualifier trees.
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
    
        serializer.serialize ( "description",       this->mDescription );
        serializer.serialize ( "weight",            this->mWeight );
        serializer.serialize ( "maturity",          this->mMaturity );
        serializer.serialize ( "args",              this->mArgs );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        assert ( false ); // unsupported
    }
};

//================================================================//
// Schema
//================================================================//
class Schema :
    public AbstractSerializable {
private:

    friend class Ledger;
    friend class SchemaLua;

    string          mName;

    SerializableMap < string, SchemaAssetTemplate >     mAssetTemplates;
    SerializableMap < string, SchemaAsset >             mAssetDefinitions;
    SerializableMap < string, SchemaMethod >            mMethods;

    string          mLua;

    //----------------------------------------------------------------//
    const SchemaMethod*     getMethod           ( string name ) const;

    //----------------------------------------------------------------//
    void                    AbstractSerializable_serializeFrom      ( const AbstractSerializerFrom& serializer ) override;
    void                    AbstractSerializable_serializeTo        ( AbstractSerializerTo& serializer ) const override;

public:

    //----------------------------------------------------------------//
                            Schema              ();
    bool                    verifyMethod        ( string methodName, u64 weight, u64 maturity ) const;
};

} // namespace Volition
#endif
