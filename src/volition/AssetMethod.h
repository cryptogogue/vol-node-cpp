// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_ASSETMETHOD_H
#define VOLITION_ASSETMETHOD_H

#include <volition/common.h>
#include <volition/serialization.h>
#include <volition/SquapFactory.h>

namespace Volition {

//================================================================//
// AssetMethodConstraint
//================================================================//
class AssetMethodConstraint :
     public AbstractSerializable {
public:

    string                                                      mDescription;
    SerializableSharedPtr < AbstractSquap, SquapFactory >       mConstraint;
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
    
        serializer.serialize ( "description",       this->mDescription );
        serializer.serialize ( "constraint",        this->mConstraint );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        
        serializer.serialize ( "description",       this->mDescription );
        serializer.serialize ( "constraint",        this->mConstraint );
    }
};

//================================================================//
// AssetMethod
//================================================================//
class AssetMethod :
     public AbstractSerializable {
public:

    string      mDescription;       // friendly description for the rule.
    u64         mWeight;
    u64         mMaturity;
    
    // args are broken up into three sections for more efficient processing. this is for the benefit of wallet software, which
    // must determine what methods are valid given arbitrary user inventories. without supporting a single-asset broad phase,
    // every permutation of the entire inventory would need to be evaluated.
    SerializableMap < string, SerializableSharedPtr < AbstractSquap, SquapFactory >>        mAssetArgs;         // qualifiers for *single* assets.
    SerializableMap < string, SerializableSharedPtr < AbstractSquap, SquapFactory >>        mConstArgs;         // user provided consts. may reference assets as constraints.
    SerializableVector < SerializableSharedPtr < AssetMethodConstraint >>                  mConstraints;       // constraints on groups of assets.
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
    
        serializer.serialize ( "description",       this->mDescription );
        serializer.serialize ( "weight",            this->mWeight );
        serializer.serialize ( "maturity",          this->mMaturity );
        serializer.serialize ( "assetArgs",         this->mAssetArgs );
        serializer.serialize ( "constArgs",         this->mConstArgs );
        serializer.serialize ( "constraints",       this->mConstraints );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        
        serializer.serialize ( "description",       this->mDescription );
        serializer.serialize ( "weight",            this->mWeight );
        serializer.serialize ( "maturity",          this->mMaturity );
        serializer.serialize ( "assetArgs",         this->mAssetArgs );
        serializer.serialize ( "constArgs",         this->mConstArgs );
        serializer.serialize ( "constraints",       this->mConstraints );
    }
};

} // namespace Volition
#endif
