// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_ASSET_H
#define VOLITION_ASSET_H

#include <volition/common.h>
#include <volition/AssetDefinition.h>
#include <volition/AssetFieldValue.h>
#include <volition/Format.h>
#include <volition/serialization/Serialization.h>

namespace Volition {

//================================================================//
// Asset
//================================================================//
class Asset :
     public AbstractSerializable {
public:

    typedef map < string, AssetFieldValue > Fields;
    typedef u64 Index;

    enum {
        NULL_ASSET_ID = ( u64 )-1,
    };

    Index       mIndex;
    
    string      mType;
    string      mOwner;
    Fields      mFields;
    
    shared_ptr < AssetDefinition > mDefinition;
    
    //----------------------------------------------------------------//
    Asset () {
    }
    
    //----------------------------------------------------------------//
    Asset ( shared_ptr < AssetDefinition > definition ) :
        mDefinition ( definition ) {
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
        
        string assetID;
        serializer.serialize ( "assetID", assetID );
        this->mIndex = stoi ( assetID );
        
        serializer.serialize ( "type",      this->mType );
        serializer.serialize ( "owner",     this->mOwner );
        
        serializer.context ( "fields", [ this ]( const AbstractSerializerFrom& serializer ) {
            
            assert ( this->mDefinition );
        
            AssetDefinition::Fields::const_iterator definitionFieldsIt = this->mDefinition->mFields.cbegin ();
            for ( ; definitionFieldsIt != this->mDefinition->mFields.cend (); ++definitionFieldsIt ) {
            
                string fieldName = definitionFieldsIt->first;
                const AssetDefinitionField& definitionField = definitionFieldsIt->second;
                
                this->mFields [ fieldName ].serializeValue ( serializer, definitionField.getType (), fieldName );
            }
        });
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        
        string assetID = Format::write ( "%d", this->mIndex );
        
        serializer.serialize ( "assetID",   assetID );
        serializer.serialize ( "type",      this->mType );
        serializer.serialize ( "owner",     this->mOwner );
        
        serializer.context ( "fields", [ this ]( AbstractSerializerTo& serializer ) {
            
            assert ( this->mDefinition );
        
            map < string, AssetFieldValue >::const_iterator fieldsIt = this->mFields.cbegin ();
            for ( ; fieldsIt != this->mFields.cend (); ++fieldsIt ) {
                fieldsIt->second.serializeValue ( serializer, fieldsIt->first );
            }
        });
    }
};

} // namespace Volition
#endif