// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SERIALIZATION_SERIALIZABLESET_H
#define VOLITION_SERIALIZATION_SERIALIZABLESET_H

#include <volition/serialization/AbstractSerializable.h>
#include <volition/serialization/AbstractSerializer.h>

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
    void AbstractSerializable_serialize ( AbstractSerializer& serializer ) {
        
        switch ( serializer.getMode ()) {
            case AbstractSerializer::SERIALIZE_DIGEST: {
                // TODO: sort set elements and hash
                break;
            }
            case AbstractSerializer::SERIALIZE_IN: {
                size_t size = serializer.getSize ();
                for ( size_t i = 0; i < size; ++i ) {
                    TYPE value;
                    serializer.serialize ( i, value );
                    this->insert ( value );
                }
                break;
            }
            case AbstractSerializer::SERIALIZE_OUT: {
                typename set < TYPE >::iterator it = this->begin ();
                for ( size_t i = 0; it != this->end (); ++it, ++i ) {
                    TYPE value = *it;
                    serializer.serialize ( i, value );
                }
                break;
            }
        }
    }
};

} // namespace Volition
#endif
