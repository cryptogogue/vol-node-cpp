// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_ASSET_H
#define VOLITION_ASSET_H

#include <volition/common.h>
#include <volition/AssetDefinition.h>
#include <volition/AssetFieldValue.h>
#include <volition/AssetID.h>
#include <volition/Format.h>
#include <volition/serialization/Serialization.h>

namespace Volition {

//================================================================//
// Asset
//================================================================//
class Asset :
     public AbstractSerializable {
public:

    typedef SerializableMap < string, AssetFieldValue > Fields;

    enum {
        NULL_POSITION   = ( u64 )-1,
    };
    
    AssetID     mAssetID;
    
    string      mType;
    string      mOwner;
    u64         mInventoryNonce;
    Fields      mFields;
        
    //----------------------------------------------------------------//
    Asset () {
    }
    
    //----------------------------------------------------------------//
    AssetFieldValue getField ( string name ) const {
    
        if ( name == "@" ) {
            return AssetFieldValue ( this->mType );
        }
    
        Fields::const_iterator fieldIt = this->mFields.find ( name );
        if ( fieldIt != this->mFields.cend ()) {
            return fieldIt->second;
        }
        return AssetFieldValue ();
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        
        serializer.serialize ( "assetID",           this->mAssetID );
        serializer.serialize ( "type",              this->mType );
        serializer.serialize ( "owner",             this->mOwner );
        serializer.serialize ( "inventoryNonce",    this->mInventoryNonce );
        serializer.serialize ( "fields",            this->mFields );
        
//        serializer.context ( "fields", [ this ]( const AbstractSerializerFrom& serializer )
//
//        serializer.context ( "fields", [ this ]( const AbstractSerializerFrom& serializer ) {
//
//            assert ( this->mDefinition );
//
//            AssetDefinition::Fields::const_iterator definitionFieldsIt = this->mDefinition->mFields.cbegin ();
//            for ( ; definitionFieldsIt != this->mDefinition->mFields.cend (); ++definitionFieldsIt ) {
//
//                string fieldName = definitionFieldsIt->first;
//                const AssetFieldDefinition& definitionField = definitionFieldsIt->second;
//                
//                this->mFields [ fieldName ].serializeValue ( serializer, definitionField.getType (), fieldName );
//            }
//        });
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        
        serializer.serialize ( "assetID",           this->mAssetID );
        serializer.serialize ( "type",              this->mType );
        serializer.serialize ( "owner",             this->mOwner );
        serializer.serialize ( "inventoryNonce",    this->mInventoryNonce );
        serializer.serialize ( "fields",            this->mFields );
        
//        serializer.context ( "fields", [ this ]( AbstractSerializerTo& serializer ) {
//
//            assert ( this->mDefinition );
//
//            map < string, AssetFieldValue >::const_iterator fieldsIt = this->mFields.cbegin ();
//            for ( ; fieldsIt != this->mFields.cend (); ++fieldsIt ) {
//                fieldsIt->second.serializeValue ( serializer, fieldsIt->first );
//            }
//        });
    }
};

} // namespace Volition
#endif
