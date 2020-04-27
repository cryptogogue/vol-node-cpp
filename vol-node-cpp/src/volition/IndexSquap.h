// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_INDEXSQUAP_H
#define VOLITION_INDEXSQUAP_H

#include <volition/common.h>
#include <volition/SquapFactory.h>

namespace Volition {

//================================================================//
// IndexSquap
//================================================================//
class IndexSquap :
     public AbstractSquap {
public:

    string  mArgName; // unused for now
    string  mIndexer;
    
    //----------------------------------------------------------------//
    AssetFieldValue AbstractSquap_evaluate ( const SquapEvaluationContext& context ) const override {
        
        // TODO: right now, only supporting single assets
        const Asset* asset = context.getAsset ();
        if ( !asset ) return AssetFieldValue ();
        
        return asset->getField ( this->mIndexer );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
    
        serializer.serialize ( "paramID",       this->mArgName );
        serializer.serialize ( "value",         this->mIndexer );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractSquap::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "paramID",       this->mArgName );
        serializer.serialize ( "value",         this->mIndexer );
    }
};

} // namespace Volition
#endif
