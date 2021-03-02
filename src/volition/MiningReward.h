// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_MININGREWARD_H
#define VOLITION_MININGREWARD_H

#include <volition/common.h>
#include <volition/AssetFieldValue.h>
#include <volition/serialization/Serialization.h>
#include <volition/SquapFactory.h>

namespace Volition {

//================================================================//
// MiningReward
//================================================================//
class MiningReward :
     public AbstractSerializable {
public:

    string          mFriendlyName;
    string          mDescription;       // friendly description for the reward.
    string          mLua;
    u64             mQuantity;
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
    
        this->mQuantity = 0; // fallback
    
        serializer.serialize ( "friendlyName",      this->mFriendlyName );
        serializer.serialize ( "description",       this->mDescription );
        serializer.serialize ( "lua",               this->mLua );
        serializer.serialize ( "quantity",          this->mQuantity );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        
        serializer.serialize ( "friendlyName",      this->mFriendlyName );
        serializer.serialize ( "description",       this->mDescription );
        serializer.serialize ( "lua",               this->mLua );
        serializer.serialize ( "quantity",          this->mQuantity );
    }
};

} // namespace Volition
#endif
