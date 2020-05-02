// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_ASSETID_H
#define VOLITION_ASSETID_H

#include <volition/common.h>
#include <volition/serialization/Serialization.h>

namespace Volition {

//================================================================//
// AssetID
//================================================================//
class AssetID :
    public AbstractSerializable {
public:

    typedef u64 Index;
    enum {
        NULL_INDEX      = ( u64 )-1,
    };

    Index   mIndex;

    //----------------------------------------------------------------//
    static u64          decode          ( string assetID, bool* isValid = NULL );
    static string       encode          ( u64 index );
    
    //----------------------------------------------------------------//
    operator Index () const {
        return this->mIndex;
    }
    
    //----------------------------------------------------------------//
    operator string () const {
        return AssetID::encode ( this->mIndex );
    }
    
    //----------------------------------------------------------------//
    AssetID& operator = ( Index index ) {
        this->mIndex = index;
        return *this;
    }
    
    //----------------------------------------------------------------//
    AssetID& operator = ( string assetID ) {
        this->mIndex = AssetID::decode ( assetID );
        return *this;
    }
    
    //----------------------------------------------------------------//
    AssetID& operator = ( const AssetID& assetID ) {
        this->mIndex = assetID.mIndex;
        return *this;
    }
    
    //----------------------------------------------------------------//
    AssetID () :
        mIndex ( NULL_INDEX ) {
    }
    
    //----------------------------------------------------------------//
    AssetID ( Index index ) :
        mIndex ( index ) {
    }
    
    //----------------------------------------------------------------//
    AssetID ( const string& assetID ) :
        mIndex ( AssetID::decode ( assetID )) {
    }
    
    //----------------------------------------------------------------//
    AssetID ( const AssetID& assetID ) :
        mIndex ( assetID.mIndex ) {
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        
        this->mIndex = NULL_INDEX;
        
        const AbstractSerializerFrom* parent = serializer.getParent ();
        if ( parent ) {
            string assetID;
            parent->serialize ( serializer.getName (), assetID );
            this->mIndex = AssetID::decode ( assetID );
        }
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        
        AbstractSerializerTo* parent = serializer.getParent ();
        if ( parent ) {
            string assetID = AssetID::encode ( this->mIndex );
            parent->serialize ( serializer.getName (), assetID );
        }
    }
};

} // namespace Volition
#endif
