// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_ASSETIDENTIFIER_H
#define VOLITION_ASSETIDENTIFIER_H

#include <volition/common.h>

namespace Volition {

//================================================================//
// AssetIdentifier
//================================================================//
class AssetIdentifier :
    public AbstractSerializable {
public:

    string      mType;
    size_t      mIndex;
    
    //----------------------------------------------------------------//
    AssetIdentifier () :
        mIndex ( 0 ) {
    }
    
    //----------------------------------------------------------------//
    AssetIdentifier ( string type, size_t index ) :
        mType ( type ),
        mIndex ( index ) {
    }

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        
        serializer.serialize ( "type",      this->mType );
        serializer.serialize ( "index",     this->mIndex );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        
        serializer.serialize ( "type",      this->mType );
        serializer.serialize ( "index",     this->mIndex );
    }
};

} // namespace Volition
#endif
