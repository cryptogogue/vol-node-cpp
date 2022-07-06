// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_ASSETTRANSFERDETAILS_H
#define VOLITION_ASSETTRANSFERDETAILS_H

#include <volition/common.h>
#include <volition/serialization/Serialization.h>

namespace Volition {

//================================================================//
// AssetTransferDetails
//================================================================//
class AssetTransferDetails :
    public AbstractSerializable {
public:

    string          mFrom;
    AccountID       mFromID;

    string          mTo;
    AccountID       mToID;

    SerializableList < SerializableSharedConstPtr < Asset >>    mAssets;

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        
        serializer.serialize ( "from",              this->mFrom );
        serializer.serialize ( "fromID",            this->mFromID );
        serializer.serialize ( "to",                this->mTo );
        serializer.serialize ( "toID",              this->mToID );
        serializer.serialize ( "assets",            this->mAssets );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        
        serializer.serialize ( "from",              this->mFrom );
        serializer.serialize ( "fromID",            this->mFromID );
        serializer.serialize ( "to",                this->mTo );
        serializer.serialize ( "toID",              this->mToID );
        serializer.serialize ( "assets",            this->mAssets );
    }
    
    //----------------------------------------------------------------//
    AssetTransferDetails () {
    }
    
    //----------------------------------------------------------------//
    ~AssetTransferDetails () {
    }
};

} // namespace Volition
#endif
