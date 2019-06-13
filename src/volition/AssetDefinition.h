// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_ASSETDEFINITION_H
#define VOLITION_ASSETDEFINITION_H

#include <volition/common.h>
#include <volition/serialization/Serialization.h>
#include <volition/AssetFieldValue.h>

namespace Volition {

//================================================================//
// AssetDefinitionField
//================================================================//
class AssetDefinitionField :
    public AssetFieldValue {
public:

    bool    mMutable;

    //----------------------------------------------------------------//
    AssetDefinitionField () :
        mMutable ( false ) {
    }

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AssetFieldValue::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "mutable",   this->mMutable );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AssetFieldValue::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "mutable",   this->mMutable );
    }
};

//================================================================//
// AssetDefinition
//================================================================//
class AssetDefinition :
     public AbstractSerializable {
public:

    typedef SerializableMap < string, AssetDefinitionField > Fields;

    Fields      mFields;        // default values for fields
    
    //----------------------------------------------------------------//
    AssetDefinitionField getField ( string fieldName ) const {
    
        Fields::const_iterator fieldsIt = this->mFields.find ( fieldName );
        if ( fieldsIt != this->mFields.cend ()) {
            return fieldsIt->second;
        }
        return AssetDefinitionField ();
    }
    
    //----------------------------------------------------------------//
    bool hasMutableField ( string name, AssetFieldValue::Type type ) const {
    
        Fields::const_iterator fieldIt = this->mFields.find ( name );
        if ( fieldIt != this->mFields.cend ()) {
            const AssetDefinitionField& field = fieldIt->second;
            return ( field.mMutable && ( field.getType () == type ));
        }
        return false;
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
    
        serializer.serialize ( "fields",        this->mFields );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        
        serializer.serialize ( "fields",        this->mFields );
    }
};

} // namespace Volition
#endif
