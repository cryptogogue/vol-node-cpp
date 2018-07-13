// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SERIALIZATION_SERIALIZABLEVECTOR_H
#define VOLITION_SERIALIZATION_SERIALIZABLEVECTOR_H

#include <volition/serialization/AbstractSerializable.h>
#include <volition/serialization/AbstractSerializer.h>

namespace Volition {

//================================================================//
// SerializableVector
//================================================================//
template < typename TYPE >
class SerializableVector :
    public AbstractSerializableArray,
    public vector < TYPE > {
public:

    //----------------------------------------------------------------//
    void AbstractSerializableArray_resize ( size_t size ) {
        this->resize ( size );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serialize ( AbstractSerializer& serializer ) {
        size_t size = this->size ();
        for ( size_t i = 0; i < size; ++i ) {
            serializer.serialize ( i, ( *this )[ i ]);
        }
    }
};

} // namespace Volition
#endif
