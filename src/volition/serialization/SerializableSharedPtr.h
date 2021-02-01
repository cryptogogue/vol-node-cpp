// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SERIALIZATION_SERIALIZABLESHAREDPTR_H
#define VOLITION_SERIALIZATION_SERIALIZABLESHAREDPTR_H

#include <volition/serialization/SerializablePtrFactory.h>

namespace Volition {

//================================================================//
// BaseSerializableSharedPtr
//================================================================//
template < typename TYPE, typename FACTORY_PRODUCT_TYPE, typename FACTORY = SerializablePtrFactory < FACTORY_PRODUCT_TYPE >>
class BaseSerializableSharedPtr :
    public AbstractSerializable,
    public shared_ptr < TYPE > {
public:

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) {
        
        FACTORY factory;
        unique_ptr < FACTORY_PRODUCT_TYPE > value = factory.make ( serializer );
        
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
    BaseSerializableSharedPtr () {
    }
    
    //----------------------------------------------------------------//
    BaseSerializableSharedPtr ( shared_ptr < TYPE >& other ) :
        shared_ptr < TYPE >( other ) {
    }
    
    //----------------------------------------------------------------//
    BaseSerializableSharedPtr ( const shared_ptr < TYPE >& other ) :
        shared_ptr < TYPE >( other ) {
    }
};

template < typename TYPE, typename FACTORY = SerializablePtrFactory < TYPE >>
using SerializableSharedConstPtr = BaseSerializableSharedPtr < const TYPE, TYPE, FACTORY >;

template < typename TYPE, typename FACTORY = SerializablePtrFactory < TYPE >>
using SerializableSharedPtr = BaseSerializableSharedPtr < TYPE, TYPE, FACTORY >;

} // namespace Volition
#endif
