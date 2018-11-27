// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SERIALIZATION_SERIALIZABLEVECTOR_H
#define VOLITION_SERIALIZATION_SERIALIZABLEVECTOR_H

#include <volition/serialization/AbstractSerializerFrom.h>
#include <volition/serialization/AbstractSerializerTo.h>

namespace Volition {

//================================================================//
// SerializableVector
//================================================================//
template < typename TYPE >
class SerializableVector :
    public AbstractSerializableCollection,
    public vector < TYPE > {
    
public:
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        size_t size = this->size ();
        for ( size_t i = 0; i < size; ++i ) {
            serializer.serialize ( i, ( *this )[ i ]);
        }
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        size_t size = this->size ();
        for ( size_t i = 0; i < size; ++i ) {
            serializer.serialize ( i, ( *this )[ i ]);
        }
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializableCollection_resize ( size_t size ) override {
        this->resize ( size );
    }
};

} // namespace Volition
#endif
