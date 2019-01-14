// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SERIALIZATION_SERIALIZABLEMAP_H
#define VOLITION_SERIALIZATION_SERIALIZABLEMAP_H

#include <volition/serialization/AbstractSerializerFrom.h>
#include <volition/serialization/AbstractSerializerTo.h>

namespace Volition {

//================================================================//
// SerializableMap
//================================================================//
template < typename KEY_TYPE, typename VAL_TYPE >
class SerializableMap :
    public AbstractSerializable,
    public map < KEY_TYPE, VAL_TYPE > {
public:
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) {
        
        if ( serializer.getKeyType () != AbstractSerializerFrom::KEY_TYPE_STRING ) return;
        
        SerializerKeys keys = serializer.getKeys ();
        
        size_t size = keys.getSize ();
        for ( size_t i = 0; i < size; ++i ) {
        
            SerializerPropertyName key = keys.getKey ( i );
        
            VAL_TYPE value;
            serializer.serialize ( key, value );
            ( *this )[ key.getName ()] = value;
        }
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const {
        
        if ( serializer.isDigest ()) {
            // TODO: sort set elements and hash
        }
        else {
            typename map < KEY_TYPE, VAL_TYPE >::const_iterator it = this->begin ();
            for ( ; it != this->end (); ++it ) {
                serializer.serialize ( it->first, it->second );
            }
        }
    }
};

} // namespace Volition
#endif
