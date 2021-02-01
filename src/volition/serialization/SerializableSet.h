// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SERIALIZATION_SERIALIZABLESET_H
#define VOLITION_SERIALIZATION_SERIALIZABLESET_H

#include <volition/serialization/AbstractSerializerFrom.h>
#include <volition/serialization/AbstractSerializerTo.h>

namespace Volition {

//================================================================//
// SerializableSet
//================================================================//
template < typename TYPE >
class SerializableSet :
    public AbstractSerializable,
    public set < TYPE > {
public:
    
    //----------------------------------------------------------------//
    SerializableSet& operator = ( const set < TYPE >& other ) {
        set < TYPE >::operator = ( other );
        return *this;
    }
    
    //----------------------------------------------------------------//
    SerializableSet& operator = ( const SerializableSet < TYPE >& other ) {
        set < TYPE >::operator = ( other );
        return *this;
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) {
        
        this->clear ();
        if ( serializer.getKeyType () != AbstractSerializerFrom::KEY_TYPE_INDEX ) return;
        
        size_t size = serializer.getSize ();
        for ( size_t i = 0; i < size; ++i ) {
            TYPE value;
            serializer.serialize ( i, value );
            this->insert ( value );
        }
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const {
        
        serializer.affirmArray ();
    
        typename set < TYPE >::const_iterator it = this->begin ();
        for ( size_t i = 0; it != this->end (); ++it, ++i ) {
            serializer.serialize ( i, *it );
        }
    }
    
    //----------------------------------------------------------------//
    SerializableSet () {
    }
    
    //----------------------------------------------------------------//
    SerializableSet ( const set < TYPE >& other ) :
        set < TYPE >( other ) {
    }
    
    //----------------------------------------------------------------//
    SerializableSet ( const SerializableSet < TYPE >& other ) :
        set < TYPE >( other ) {
    }
};

} // namespace Volition
#endif
