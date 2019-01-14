// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SERIALIZATION_SERIALIZABLESHAREDPTR_H
#define VOLITION_SERIALIZATION_SERIALIZABLESHAREDPTR_H

#include <volition/serialization/SerializablePtrFactory.h>

namespace Volition {

//================================================================//
// SerializableSharedPtr
//================================================================//
template < typename TYPE, typename FACTORY = SerializablePtrFactory < TYPE >>
class SerializableSharedPtr :
    public AbstractSerializable,
    public shared_ptr < TYPE > {
public:

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) {
        
        FACTORY factory;
        unique_ptr < TYPE > value = factory.make ( serializer );
        
        if ( value ) {
            value->serializeFrom ( serializer );
        }
        *( shared_ptr < TYPE >* )this = move ( value );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const {
        
        const TYPE* value = this->get ();
        if ( value ) {
            value->serializeTo ( serializer );
        }
    }

    //----------------------------------------------------------------//
    SerializableSharedPtr () {
    }
    
    //----------------------------------------------------------------//
    SerializableSharedPtr ( shared_ptr < TYPE >& other ) :
        shared_ptr < TYPE >( other ) {
    }
    
    //----------------------------------------------------------------//
    SerializableSharedPtr ( const shared_ptr < TYPE >& other ) :
        shared_ptr < TYPE >( other ) {
    }
};

} // namespace Volition
#endif
