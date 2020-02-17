// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SERIALIZATION_SERIALIZERKEYS_H
#define VOLITION_SERIALIZATION_SERIALIZERKEYS_H

#include <volition/common.h>
#include <volition/serialization/SerializerPropertyName.h>

namespace Volition {

//================================================================//
// SerializerKeys
//================================================================//
class SerializerKeys {
protected:

    bool                mIsIndex;
    size_t              mSize;
    vector < string >   mKeys;

public:

    //----------------------------------------------------------------//
    SerializerPropertyName getKey ( size_t index ) const {
    
        if ( index < this->mSize ) {
            if ( this->mIsIndex ) {
                return SerializerPropertyName ( index );
            }
            assert ( index < this->mKeys.size ());
            return SerializerPropertyName ( this->mKeys [ index ]);
        }
        return SerializerPropertyName (( size_t )-1 ); // TODO: fix this up
    }

    //----------------------------------------------------------------//
    size_t getSize () const {
        return this->mSize;
    }
    
    //----------------------------------------------------------------//
    bool isIndex () const {
        return this->mIsIndex;
    }
    
    //----------------------------------------------------------------//
    SerializerKeys () :
        mIsIndex ( false ),
        mSize ( 0 ) {
    }
    
    //----------------------------------------------------------------//
    SerializerKeys ( size_t size ) :
        mIsIndex ( true ),
        mSize ( size ) {
    }
    
    //----------------------------------------------------------------//
    SerializerKeys ( vector < string > keys ) :
        mIsIndex ( false ),
        mSize ( keys.size ()),
        mKeys ( keys ) {
    }
};

} // namespace Volition
#endif
