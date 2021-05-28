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

    string  mArgName;
    string  mIndexer;
    
    //----------------------------------------------------------------//
    AssetFieldValue AbstractSquap_evaluate ( const SquapEvaluationContext& context ) const override {
        
        const Asset* asset = context.getAsset ( this->mArgName );
        if ( asset ) {
            if ( this->mIndexer.size ()) {
                return asset->getField ( this->mIndexer );
            }
            return asset->mType;
        }
        return context.getValue ( this->mArgName );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
    
        serializer.serialize ( "argName",       this->mArgName );
        serializer.serialize ( "indexer",       this->mIndexer );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractSquap::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "argName",       this->mArgName );
        serializer.serialize ( "indexer",       this->mIndexer );
    }
};

} // namespace Volition
#endif
