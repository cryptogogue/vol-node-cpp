// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SERIALIZATION_SERIALIZABLESET_H
#define VOLITION_SERIALIZATION_SERIALIZABLESET_H

#include <volition/serialization/AbstractSerializable.h>
#include <volition/serialization/AbstractSerializableArray.h>
#include <volition/serialization/AbstractSerializerFrom.h>
#include <volition/serialization/AbstractSerializerTo.h>

namespace Volition {

//================================================================//
// SerializableSet
//================================================================//
template < typename TYPE >
class SerializableSet :
    public AbstractSerializableArray,
    public set < TYPE > {
public:

    //----------------------------------------------------------------//
    void AbstractSerializableArray_resize ( size_t size ) {
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) {
        
        size_t size = serializer.getSize ();
        for ( size_t i = 0; i < size; ++i ) {
            TYPE value;
            serializer.serialize ( i, value );
            this->insert ( value );
        }
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const {
        
        if ( serializer.isDigest ()) {
            // TODO: sort set elements and hash
        }
        else {
            typename set < TYPE >::iterator it = this->begin ();
            for ( size_t i = 0; it != this->end (); ++it, ++i ) {
                TYPE value = *it;
                serializer.serialize ( i, value );
            }
        }
    }
};

} // namespace Volition
#endif
