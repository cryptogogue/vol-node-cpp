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
class Asset :
     public AbstractSerializable {
public:

    string                                  mType;
    string                                  mOwner;
    SerializableMap < string, Variant >     mFields;
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
    
        serializer.serialize ( "type",              this->mType );
        serializer.serialize ( "owner",             this->mOwner );
        serializer.serialize ( "fields",            this->mFields );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
    
        serializer.serialize ( "type",              this->mType );
        serializer.serialize ( "owner",             this->mOwner );
        serializer.serialize ( "fields",            this->mFields );
    }
};

//================================================================//
// Inventory
//================================================================//
typedef SerializableVector < Asset > Inventory;

} // namespace Volition
#endif
