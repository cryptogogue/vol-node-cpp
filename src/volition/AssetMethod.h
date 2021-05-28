// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_ASSETMETHOD_H
#define VOLITION_ASSETMETHOD_H

#include <volition/common.h>
#include <volition/AssetFieldValue.h>
#include <volition/serialization/Serialization.h>
#include <volition/SquapFactory.h>

namespace Volition {

//================================================================//
// AssetMethodParamDesc
//================================================================//
class AssetMethodParamDesc :
    public AbstractSerializable {
public:

    SerializableSharedPtr < AbstractSquap, SquapFactory >   mQualifier;
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        
        serializer.serialize ( "qualifier",     this->mQualifier );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        
        serializer.serialize ( "qualifier",     this->mQualifier );
    }
};

//================================================================//
// ConstMethodParamDesc
//================================================================//
class ConstMethodParamDesc :
    public AbstractSerializable {
public:

    SerializableSharedPtr < AbstractSquap, SquapFactory >   mQualifier;
    AssetFieldValue                                         mDefaultValue;
    SerializableOpaque                                      mInputScheme;
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        
        serializer.serialize ( "qualifier",     this->mQualifier );
        serializer.serialize ( "defaultValue",  this->mDefaultValue );
        serializer.serialize ( "inputScheme",   this->mInputScheme );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        
        serializer.serialize ( "qualifier",     this->mQualifier );
        serializer.serialize ( "defaultValue",  this->mDefaultValue );
        serializer.serialize ( "inputScheme",   this->mInputScheme );
    }
};

//================================================================//
// AssetMethod
//================================================================//
class AssetMethod :
     public AbstractSerializable {
public:

    typedef SerializableMap < string, AssetMethodParamDesc > AssetArgs;
    typedef SerializableMap < string, ConstMethodParamDesc > ConstArgs;
    typedef SerializableVector < SerializableSharedPtr < AbstractSquap, SquapFactory >> Constraints;

    string          mFriendlyName;
    string          mDescription;   // friendly description for the method.
    u64             mWeight;
    u64             mMaturity;
    
    AssetArgs       mAssetArgs;     // qualifiers for asset args.
    ConstArgs       mConstArgs;     // qualifiers for const args.
    Constraints     mConstraints;   // constraints on groups of assets.
    
    // the standalone Lua script to run.
    string          mLua;
    
    //----------------------------------------------------------------//
    bool            checkInvocation             ( const map < string, shared_ptr < const Asset >>& assetParams, const map < string, AssetFieldValue >& constParams ) const;

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
