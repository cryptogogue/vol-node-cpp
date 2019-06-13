// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_ASSET_H
#define VOLITION_ASSET_H

#include <volition/common.h>
#include <volition/AssetDefinition.h>
#include <volition/AssetFieldValue.h>
#include <volition/serialization/Serialization.h>

namespace Volition {

//================================================================//
// AssetFields
//================================================================//
class AssetFields :
    public AbstractSerializable,
    public map < string, AssetFieldValue > {
public:
    
    // TODO: nuke this class in favor of serializer push/pop context (to be written)
    
    shared_ptr < AssetDefinition > mDefinition;

    //----------------------------------------------------------------//
    AssetFields () {
    }
    
    //----------------------------------------------------------------//
    AssetFields ( const map < string, AssetFieldValue >& other ) :
        map < string, AssetFieldValue >( other ) {
    }
    
    //----------------------------------------------------------------//
    AssetFields ( const AssetFields& other ) :
        map < string, AssetFieldValue >( other ),
        mDefinition ( other.mDefinition ) {
    }

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
    
        assert ( this->mDefinition );
        
        AssetDefinition::Fields::const_iterator definitionFieldsIt = this->mDefinition->mFields.cbegin ();
        for ( ; definitionFieldsIt != this->mDefinition->mFields.cend (); ++definitionFieldsIt ) {
        
            string fieldName = definitionFieldsIt->first;
            const AssetDefinitionField& definitionField = definitionFieldsIt->second;
            
            ( *this )[ fieldName ].serializeValue ( serializer, definitionField.getType (), fieldName );
        }
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
    
        assert ( this->mDefinition );
        
        map < string, AssetFieldValue >::const_iterator fieldsIt = this->cbegin ();
        for ( ; fieldsIt != this->cend (); ++fieldsIt ) {
            fieldsIt->second.serializeValue ( serializer, fieldsIt->first );
        }
    }
};

//================================================================//
// Asset
//================================================================//
class Asset :
     public AbstractSerializable {
public:

    typedef AssetFields Fields;
    typedef u64 Index;

    enum {
        NULL_ASSET_ID = ( u64 )-1,
    };

    Index       mIndex;
    
    string      mType;
    string      mOwner;
    Fields      mFields;
        
    //----------------------------------------------------------------//
    Asset () {
    }
    
    //----------------------------------------------------------------//
    Asset ( shared_ptr < AssetDefinition > definition ) {
        this->mFields.mDefinition = definition;
    }
    
    //----------------------------------------------------------------//
    AssetFieldValue getField ( string name ) const {
    
        Fields::const_iterator fieldIt = this->mFields.find ( name );
        if ( fieldIt != this->mFields.cend ()) {
            return fieldIt->second;
        }
        return AssetFieldValue ();
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        
        serializer.serialize ( "index",             this->mIndex );
        serializer.serialize ( "type",              this->mType );
        serializer.serialize ( "owner",             this->mOwner );
        serializer.serialize ( "fields",            this->mFields );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        
        serializer.serialize ( "index",             this->mIndex );
        serializer.serialize ( "type",              this->mType );
        serializer.serialize ( "owner",             this->mOwner );
        serializer.serialize ( "fields",            this->mFields );
    }
};

} // namespace Volition
#endif
