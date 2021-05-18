// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_INDEXID_H
#define VOLITION_INDEXID_H

#include <volition/common.h>
#include <volition/serialization/Serialization.h>

namespace Volition {

//================================================================//
// IndexID
//================================================================//
class IndexID :
    public AbstractSerializable {
public:

    typedef u64 Index;
    enum {
        NULL_INDEX      = ( u64 )-1,
    };

    Index   mIndex;
    
    //----------------------------------------------------------------//
    operator Index () const {
        return this->mIndex;
    }
    
    //----------------------------------------------------------------//
    IndexID& operator = ( Index index ) {
        this->mIndex = index;
        return *this;
    }
    
    //----------------------------------------------------------------//
    IndexID& operator = ( const IndexID& accountID ) {
        this->mIndex = accountID.mIndex;
        return *this;
    }
    
    //----------------------------------------------------------------//
    IndexID () :
        mIndex ( NULL_INDEX ) {
    }
    
    //----------------------------------------------------------------//
    IndexID ( Index index ) :
        mIndex ( index ) {
    }
    
    //----------------------------------------------------------------//
    IndexID ( const IndexID& accountID ) :
        mIndex ( accountID.mIndex ) {
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        
        this->mIndex = NULL_INDEX;
        
        const AbstractSerializerFrom* parent = serializer.getParent ();
        if ( parent ) {
            parent->serialize ( serializer.getName (), this->mIndex );
        }
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        
        AbstractSerializerTo* parent = serializer.getParent ();
        if ( parent ) {
            parent->serialize ( serializer.getName (), this->mIndex );
        }
    }
};

typedef IndexID AccountID;
typedef IndexID OfferID;

} // namespace Volition
#endif
