// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SERIALIZATION_SERIALIZABLEUNIQUEPTR_H
#define VOLITION_SERIALIZATION_SERIALIZABLEUNIQUEPTR_H

#include <volition/serialization/SerializablePtrFactory.h>

namespace Volition {

//================================================================//
// SerializableUniquePtr
//================================================================//
template < typename TYPE, typename FACTORY = SerializablePtrFactory < TYPE >>
class SerializableUniquePtr :
    public AbstractSerializable,
    public unique_ptr < TYPE > {
public:

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) {
        
        FACTORY factory;
        unique_ptr < TYPE > value = factory.make ( serializer );
        
        if ( value ) {
            value->serializeFrom ( serializer );
        }
        *( unique_ptr < TYPE >* )this = move ( value );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const {
        
        const TYPE* value = this->get ();
        if ( value ) {
            value->serializeTo ( serializer );
        }
    }

    //----------------------------------------------------------------//
    SerializableUniquePtr () {
    }
    
    //----------------------------------------------------------------//
    SerializableUniquePtr ( unique_ptr < TYPE > other ) :
        unique_ptr < TYPE >( move ( other )) {
    }
};

} // namespace Volition
#endif
