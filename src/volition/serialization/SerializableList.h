// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SERIALIZATION_SERIALIZABLELIST_H
#define VOLITION_SERIALIZATION_SERIALIZABLELIST_H

#include <volition/serialization/AbstractSerializerFrom.h>
#include <volition/serialization/AbstractSerializerTo.h>

namespace Volition {

//================================================================//
// SerializableSet
//================================================================//
template < typename TYPE >
class SerializableList :
    public AbstractSerializable,
    public list < TYPE > {
public:
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) {
        
        this->clear ();
        if ( serializer.getKeyType () != AbstractSerializerFrom::KEY_TYPE_INDEX ) return;
        
        size_t size = serializer.getSize ();
        for ( size_t i = 0; i < size; ++i ) {
            TYPE value;
            serializer.serialize ( i, value );
            this->push_back ( value );
        }
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const {
        
        serializer.affirmArray ();
    
        typename list < TYPE >::const_iterator it = this->begin ();
        for ( size_t i = 0; it != this->end (); ++it, ++i ) {
            serializer.serialize ( i, *it );
        }
    }
};

} // namespace Volition
#endif
