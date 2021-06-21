// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_STAMP_H
#define VOLITION_STAMP_H

#include <volition/common.h>
#include <volition/AssetDefinition.h>
#include <volition/AssetFieldValue.h>
#include <volition/AssetID.h>
#include <volition/Format.h>
#include <volition/serialization/Serialization.h>
#include <volition/SquapEvaluationContext.h>

namespace Volition {

//================================================================//
// Stamp
//================================================================//
class Stamp :
     public AbstractSerializable {
public:

    typedef SerializableMap < string, AssetFieldValue > Fields;
    
    SerializableSharedPtr < AbstractSquap, SquapFactory >   mQualifier;     // asset qualifier
    Fields                                                  mFields;        // fields to overwrite
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        
        serializer.serialize ( "qualifier",     this->mQualifier );
        serializer.serialize ( "fields",        this->mFields );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        
        serializer.serialize ( "qualifier",     this->mQualifier );
        
        if ( this->mFields.size ()) {
            serializer.serialize ( "fields", this->mFields );
        }
    }
    
    //----------------------------------------------------------------//
    bool checkAsset ( shared_ptr < const Asset > assetParam ) const {
        
        return this->mQualifier ? ( bool )this->mQualifier->evaluate ( SquapEvaluationContext ( assetParam )) : true;
    }
    
    //----------------------------------------------------------------//
    AssetFieldValue getField ( string name ) const {
        
        Fields::const_iterator fieldIt = this->mFields.find ( name );
        if ( fieldIt != this->mFields.cend ()) {
            return fieldIt->second;
        }
        return AssetFieldValue ();
    }
    
    //----------------------------------------------------------------//
    Stamp () {
    }
};

} // namespace Volition
#endif
