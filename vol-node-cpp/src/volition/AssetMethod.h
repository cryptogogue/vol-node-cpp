// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_ASSETMETHOD_H
#define VOLITION_ASSETMETHOD_H

#include <volition/common.h>
#include <volition/serialization/Serialization.h>
#include <volition/SquapFactory.h>

namespace Volition {

//================================================================//
// AssetMethodParamDesc
//================================================================//
class AssetMethodParamDesc :
    public AbstractSerializable {
public:

    enum Type : u64 {
        TYPE_ASSET      = FNV1a::const_hash_64 ( "ASSET" ),
        TYPE_CONST      = FNV1a::const_hash_64 ( "CONST" ),
    };

    SerializableSharedPtr < AbstractSquap, SquapFactory >   mQualifier;
    Type                                                    mType;
    SerializableOpaque                                      mInputScheme;
    
    //----------------------------------------------------------------//
    static string getTypeName ( Type type ) {
    
        switch ( type ) {
            case TYPE_ASSET:        return "ASSET";
            case TYPE_CONST:        return "CONST";
        }
        return "";
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        
        string typeStr;
        serializer.serialize ( "type", typeStr );
        this->mType = ( Type )FNV1a::hash_64 ( typeStr.c_str ());
        
        serializer.serialize ( "qualifier",     this->mQualifier );
        serializer.serialize ( "inputScheme",   this->mInputScheme );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        
        string typeStr = AssetMethodParamDesc::getTypeName ( this->mType );
        serializer.serialize ( "type", typeStr );
        
        serializer.serialize ( "qualifier",     this->mQualifier );
        serializer.serialize ( "inputScheme",   this->mInputScheme );
    }
};

//================================================================//
// AssetMethod
//================================================================//
class AssetMethod :
     public AbstractSerializable {
public:

    typedef SerializableMap < string, AssetMethodParamDesc > Arguments;
    typedef SerializableVector < SerializableSharedPtr < AbstractSquap, SquapFactory >> Constraints;

    string          mFriendlyName;
    string          mDescription;       // friendly description for the method.
    u64             mWeight;
    u64             mMaturity;
    
    // args are broken up into three sections for more efficient processing. this is for the benefit of wallet software, which
    // must determine what methods are valid given arbitrary user inventories. without supporting a single-asset broad phase,
    // every permutation of the entire inventory would need to be evaluated.
    Arguments       mAssetArgs;     // qualifiers for asset args.
    Arguments       mConstArgs;     // qualifiers for const args.
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
