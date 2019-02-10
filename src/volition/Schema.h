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
        
        serializer.serialize ( "type",              this->mType );
        serializer.serialize ( "array",             this->mArray );
        serializer.serialize ( "mutable",           this->mMutable );
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
        
        serializer.serialize ( "extends",           this->mExtends );
        serializer.serialize ( "fields",            this->mFields );
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
        
        serializer.serialize ( "implements",        this->mImplements );
        serializer.serialize ( "fields",            this->mFields );
    }
};

//================================================================//
// SchemaMethodConstraint
//================================================================//
class SchemaMethodConstraint :
     public AbstractSerializable {
public:

    string                                                      mDescription;
    SerializableSharedPtr < AbstractSquap, SquapFactory >       mConstraint;
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
    
        serializer.serialize ( "description",       this->mDescription );
        serializer.serialize ( "constraint",        this->mConstraint );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        
        serializer.serialize ( "description",       this->mDescription );
        serializer.serialize ( "constraint",        this->mConstraint );
    }
};

//================================================================//
// SchemaMethod
//================================================================//
class SchemaMethod :
     public AbstractSerializable {
public:

    string                                                                                  mDescription;       // friendly description for the rule.
    u64                                                                                     mWeight;
    u64                                                                                     mMaturity;
    
    // args are broken up into three sections for more efficient processing. this is for the benefit of wallet software, which
    // must determine what methods are valid given arbitrary user inventories. without supporting a single-asset broad phase,
    // every permutation of the entire inventory would need to be evaluated.
    SerializableMap < string, SerializableSharedPtr < AbstractSquap, SquapFactory >>        mAssetArgs;         // qualifiers for *single* assets.
    SerializableMap < string, SerializableSharedPtr < AbstractSquap, SquapFactory >>        mConstArgs;         // user provided consts. may reference assets as constraints.
    SerializableVector < SerializableSharedPtr < SchemaMethodConstraint >>                  mConstraints;       // constraints on groups of assets.
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
    
        serializer.serialize ( "description",       this->mDescription );
        serializer.serialize ( "weight",            this->mWeight );
        serializer.serialize ( "maturity",          this->mMaturity );
        serializer.serialize ( "assetArgs",         this->mAssetArgs );
        serializer.serialize ( "constArgs",         this->mConstArgs );
        serializer.serialize ( "constraints",       this->mConstraints );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        
        serializer.serialize ( "description",       this->mDescription );
        serializer.serialize ( "weight",            this->mWeight );
        serializer.serialize ( "maturity",          this->mMaturity );
        serializer.serialize ( "assetArgs",         this->mAssetArgs );
        serializer.serialize ( "constArgs",         this->mConstArgs );
        serializer.serialize ( "constraints",       this->mConstraints );
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
