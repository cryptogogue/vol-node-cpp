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

    enum Type : u64 {
        NUMERIC             = FNV1a::const_hash_64 ( "NUMERIC" ),
        STRING              = FNV1a::const_hash_64 ( "STRING" ),
    };

    Type    mType;
    bool    mMutable;
    
    //----------------------------------------------------------------//
    static string getTypeName ( Type type ) {
    
        switch ( type ) {
            case NUMERIC:       return "NUMERIC";
            case STRING:        return "STRING";
        }
        return "";
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
    
        string typeStr;
        serializer.serialize ( "type", typeStr );
        this->mType = ( Type )FNV1a::hash_64 ( typeStr.c_str ());
    
        serializer.serialize ( "mutable", this->mMutable );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        
        string typeStr = SchemaAssetTemplateField::getTypeName ( this->mType );
        
        serializer.serialize ( "type",      typeStr );
        serializer.serialize ( "mutable",   this->mMutable );
    }
};

//================================================================//
// SchemaAssetTemplate
//================================================================//
class SchemaAssetTemplate :
     public AbstractSerializable {
public:

    typedef SerializableMap < string, SchemaAssetTemplateField > Fields;

    string      mExtends;
    Fields      mFields;
    
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
// SchemaAssetDefinition
//================================================================//
class SchemaAssetDefinition :
     public AbstractSerializable {
public:

    typedef SerializableMap < string, Variant > Fields;

    string      mImplements;    // name of template
    Fields      mFields;        // default values for fields
    
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

    typedef SerializableMap < string, SchemaAssetTemplate >     AssetTemplates;
    typedef SerializableMap < string, SchemaAssetDefinition >   AssetDefinitions;
    typedef SerializableMap < string, SchemaMethod >            Methods;

    string                  mName;

    AssetTemplates          mAssetTemplates;
    AssetDefinitions        mAssetDefinitions;
    Methods                 mMethods;

    string                  mLua;

    //----------------------------------------------------------------//
    void                    composeTemplate     ( string name, SchemaAssetTemplate& assetTemplate ) const;
    const SchemaMethod*     getMethod           ( string name ) const;
    SchemaAssetTemplate     getTemplate         ( string name ) const;

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
