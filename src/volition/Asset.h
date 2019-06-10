// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_ASSET_H
#define VOLITION_ASSET_H

#include <volition/common.h>
#include <volition/AssetFieldValue.h>
#include <volition/serialization/Serialization.h>

namespace Volition {

//================================================================//
// Asset
//================================================================//
class Asset :
     public AbstractSerializable {
public:

    typedef SerializableMap < string, AssetFieldValue > Fields;

    string      mType;
    string      mOwner;
    Fields      mFields;
    
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

} // namespace Volition
#endif
