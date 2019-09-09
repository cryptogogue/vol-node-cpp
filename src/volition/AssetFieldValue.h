// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_ASSETFIELDVALUE_H
#define VOLITION_ASSETFIELDVALUE_H

#include <volition/AssetFieldVariant.h>
#include <volition/serialization/Serialization.h>

namespace Volition {

//================================================================//
// AssetFieldValue
//================================================================//
class AssetFieldValue :
    public AssetFieldVariant,
    public AbstractSerializable {
protected:

    map < string, AssetFieldVariant > mAlternatives;

public:
    
    //----------------------------------------------------------------//
    AssetFieldValue () {
    }
    
    //----------------------------------------------------------------//
    AssetFieldValue ( const AssetFieldVariant& variant ) :
        AssetFieldVariant ( variant ) {
    }
    
    //----------------------------------------------------------------//
    AssetFieldValue ( bool value ) :
        AssetFieldVariant ( value ) {
    }
    
    //----------------------------------------------------------------//
    AssetFieldValue ( double value ) :
        AssetFieldVariant ( value ) {
    }
    
    //----------------------------------------------------------------//
    AssetFieldValue ( string value ) :
        AssetFieldVariant ( value ) {
    }
    
    //----------------------------------------------------------------//
    AssetFieldValue ( Poco::Dynamic::Var var ) :
        AssetFieldVariant ( var ) {
    }
    
    //----------------------------------------------------------------//
    ~AssetFieldValue () {
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        
        string typeStr;
        serializer.serialize ( "type", typeStr );
        Type type = ( Type )FNV1a::hash_64 ( typeStr.c_str ());
        
        this->serializeValue ( serializer, type, "value" );
        
//        serializer.serialize ( "alternatives", this->mAlternatives );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        
        string typeStr = AssetFieldValue::getTypeName ( this->getType ());
        serializer.serialize ( "type", typeStr );
        
        this->serializeValue ( serializer, "value" );
        
        serializer.context ( "alternatives", [ this ]( AbstractSerializerTo& serializer ) {

            map < string, AssetFieldVariant >::const_iterator alternativesIt = this->mAlternatives.cbegin ();
            for ( ; alternativesIt != this->mAlternatives.cend (); ++alternativesIt ) {
                alternativesIt->second.serializeValue ( serializer, alternativesIt->first );
            }
        });
    }
};

} // namespace Volition
#endif
