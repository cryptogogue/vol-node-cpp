// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_ASSET_H
#define VOLITION_ASSET_H

#include <volition/common.h>
#include <volition/AssetDefinition.h>
#include <volition/AssetFieldValue.h>
#include <volition/AssetID.h>
#include <volition/Format.h>
#include <volition/IndexID.h>
#include <volition/serialization/Serialization.h>

namespace Volition {

//================================================================//
// AssetBase
//================================================================//
class AssetBase :
     public AbstractSerializable {
public:

    typedef SerializableMap < string, AssetFieldValue > Fields;
        
    string      mType;
    Fields      mFields;
    
    //----------------------------------------------------------------//
    AssetBase () {
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
        
        serializer.serialize ( "type",              this->mType );
        serializer.serialize ( "fields",            this->mFields );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        
        serializer.serialize ( "type",              this->mType );
        
        if ( this->mFields.size ()) {
            serializer.serialize ( "fields", this->mFields );
        }
    }
};

//================================================================//
// Asset
//================================================================//
class Asset :
     public AssetBase {
public:

    enum {
        NULL_POSITION   = ( u64 )-1,
    };
    
    AssetID     mAssetID;
    string      mOwner;
    u64         mInventoryNonce;
    OfferID     mOfferID;
    
    //----------------------------------------------------------------//
    Asset () {
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AssetBase::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "assetID",           this->mAssetID );
        serializer.serialize ( "owner",             this->mOwner );
        serializer.serialize ( "inventoryNonce",    this->mInventoryNonce );
        serializer.serialize ( "offerID",           this->mOfferID );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AssetBase::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "assetID",           this->mAssetID );
        serializer.serialize ( "owner",             this->mOwner );
        serializer.serialize ( "inventoryNonce",    this->mInventoryNonce );
        
        if ( this->mOfferID != OfferID::NULL_INDEX ) {
            serializer.serialize ( "offerID", this->mOfferID );
        }
    }
};

} // namespace Volition
#endif
