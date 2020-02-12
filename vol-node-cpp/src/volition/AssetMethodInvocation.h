// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_ASSETMETHODINVOCATION_H
#define VOLITION_ASSETMETHODINVOCATION_H

#include <volition/common.h>
#include <volition/AssetFieldValue.h>

namespace Volition {

//================================================================//
// AssetMethodInvocation
//================================================================//
class AssetMethodInvocation :
    public AbstractSerializable {
public:

    typedef SerializableMap < string, Asset::Index > AssetParams;
    typedef SerializableMap < string, AssetFieldValue > ConstParams;

    string          mMethodName;
    AssetParams     mAssetParams;
    ConstParams     mConstParams;
    
    u64             mWeight;
    u64             mMaturity;
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {

        serializer.serialize ( "methodName",    this->mMethodName );
        serializer.serialize ( "assetParams",   this->mAssetParams );
        serializer.serialize ( "constParams",   this->mConstParams );
        
        serializer.serialize ( "weight",        this->mWeight );
        serializer.serialize ( "maturity",      this->mMaturity );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        
        serializer.serialize ( "methodName",    this->mMethodName );
        serializer.serialize ( "assetParams",   this->mAssetParams );
        serializer.serialize ( "constParams",   this->mConstParams );
        
        serializer.serialize ( "weight",        this->mWeight );
        serializer.serialize ( "maturity",      this->mMaturity );
    }
};

} // namespace Volition
#endif
