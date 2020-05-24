// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_ASSETMETHOD_H
#define VOLITION_ASSETMETHOD_H

#include <volition/common.h>
#include <volition/serialization/Serialization.h>
#include <volition/SquapFactory.h>

namespace Volition {

//================================================================//
// AssetMethod
//================================================================//
class AssetMethod :
     public AbstractSerializable {
public:

    typedef SerializableMap < string, SerializableSharedPtr < AbstractSquap, SquapFactory >> Qualifiers;
    typedef SerializableVector < SerializableSharedPtr < AbstractSquap, SquapFactory >> Constraints;

    string          mFriendlyName;
    string          mDescription;       // friendly description for the method.
    u64             mWeight;
    u64             mMaturity;
    
    // args are broken up into three sections for more efficient processing. this is for the benefit of wallet software, which
    // must determine what methods are valid given arbitrary user inventories. without supporting a single-asset broad phase,
    // every permutation of the entire inventory would need to be evaluated.
    Qualifiers      mAssetArgs;         // qualifiers for *single* assets.
    Qualifiers      mConstArgs;         // qualifiers for user provided consts.
    Constraints     mConstraints;       // constraints on groups of assets.
    
    // the standalone Lua script to run.
    string          mLua;
    
    //----------------------------------------------------------------//
    bool            checkInvocation             ( const map < string, shared_ptr < const Asset >>& params ) const;
    bool            qualifyAssetArg             ( string argName, const Asset& asset ) const;
    bool            qualifyConstArg             ( string argName, const AssetFieldValue& value ) const;

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
    
        serializer.serialize ( "friendlyName",      this->mFriendlyName );
        serializer.serialize ( "description",       this->mDescription );
        serializer.serialize ( "weight",            this->mWeight );
        serializer.serialize ( "maturity",          this->mMaturity );
        serializer.serialize ( "assetArgs",         this->mAssetArgs );
        serializer.serialize ( "constArgs",         this->mConstArgs );
        serializer.serialize ( "constraints",       this->mConstraints );
        serializer.serialize ( "lua",               this->mLua );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        
        serializer.serialize ( "friendlyName",      this->mFriendlyName );
        serializer.serialize ( "description",       this->mDescription );
        serializer.serialize ( "weight",            this->mWeight );
        serializer.serialize ( "maturity",          this->mMaturity );
        serializer.serialize ( "assetArgs",         this->mAssetArgs );
        serializer.serialize ( "constArgs",         this->mConstArgs );
        serializer.serialize ( "constraints",       this->mConstraints );
        serializer.serialize ( "lua",               this->mLua );
    }
};

} // namespace Volition
#endif
