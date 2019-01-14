// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SERIALIZATION_SERIALIZERPROPERTYNAME_H
#define VOLITION_SERIALIZATION_SERIALIZERPROPERTYNAME_H

#include <volition/common.h>
#include <volition/serialization/AbstractSerializable.h>
#include <volition/serialization/AbstractStringifiable.h>

namespace Volition {

//================================================================//
// SerializerPropertyName
//================================================================//
class SerializerPropertyName {
protected:

    bool        mIsIndex;
    size_t      mIndex;
    string      mName;

public:

    //----------------------------------------------------------------//
    size_t getIndex () {
        assert ( this->mIsIndex );
        return this->mIndex;
    }

    //----------------------------------------------------------------//
    string getName () {
        assert ( !this->mIsIndex );
        return this->mName;
    }

    //----------------------------------------------------------------//
    bool isIndex () const {
        return this->mIsIndex;
    }

    //----------------------------------------------------------------//
    SerializerPropertyName ( size_t index ) :
        mIsIndex ( true ),
        mIndex ( index ) {
    }
    
    //----------------------------------------------------------------//
    SerializerPropertyName ( const char* name ) :
        mIsIndex ( false ),
        mName ( name ) {
    }
    
    //----------------------------------------------------------------//
    SerializerPropertyName ( string name ) :
        mIsIndex ( false ),
        mName ( name ) {
    }
};

} // namespace Volition
#endif
