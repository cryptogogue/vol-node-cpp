// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_INVENTORY_H
#define VOLITION_INVENTORY_H

#include <volition/common.h>
#include <volition/serialization/Serialization.h>

namespace Volition {

//================================================================//
// AssetIdentifier
//================================================================//
class AssetIdentifier :
     public AbstractSerializable {
private:

    friend class Ledger;

    string      mClassName;             // class this asset is based on.
    string      mSpecialization;        // identifier of the asset specialization (if any).
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
    
        serializer.serialize ( "className",         this->mClassName );
        serializer.serialize ( "specialization",    this->mSpecialization );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
    
        serializer.serialize ( "className",         this->mClassName );
        serializer.serialize ( "specialization",    this->mSpecialization );
    }
};

//================================================================//
// BulkAssetIdentifier
//================================================================//
class BulkAssetIdentifier :
    public AbstractSerializable {
private:

    friend class Ledger;

    string      mClassName;             // class this asset is based on.
    u64         mQuantity;              // quantity.
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
    
        serializer.serialize ( "className",         this->mClassName );
        serializer.serialize ( "quantity",          this->mQuantity );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
    
        serializer.serialize ( "className",         this->mClassName );
        serializer.serialize ( "quantity",          this->mQuantity );
    }
};


//================================================================//
// AssetSpecialization
//================================================================//
class AssetSpecialization :
    public AbstractSerializable {
private:

    string      mClassName;             // class this asset is based on.
    string      mBody;
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
    
        serializer.serialize ( "className",         this->mClassName );
        serializer.serialize ( "body",              this->mBody );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
    
        serializer.serialize ( "className",         this->mClassName );
        serializer.serialize ( "body",              this->mBody );
    }
};

//================================================================//
// Inventory
//================================================================//
class Inventory :
    public AbstractSerializable {
public:

    SerializableList < BulkAssetIdentifier >    mAssets;
    SerializableList < AssetSpecialization >    mSpecializations;
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
    
        serializer.serialize ( "assets",            this->mAssets );
        serializer.serialize ( "specialization",    this->mSpecializations );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
    
        serializer.serialize ( "assets",            this->mAssets );
        serializer.serialize ( "specialization",    this->mSpecializations );
    }
};

} // namespace Volition
#endif
