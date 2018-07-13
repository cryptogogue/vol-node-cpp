// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SERIALIZATION_SERIALIZABLEPTR_H
#define VOLITION_SERIALIZATION_SERIALIZABLEPTR_H

#include <volition/serialization/AbstractSerializable.h>
#include <volition/serialization/AbstractSerializer.h>

namespace Volition {

//================================================================//
// AbstractSerializablePtrFactory
//================================================================//
template < typename TYPE >
class AbstractSerializablePtrFactory {
protected:

    //----------------------------------------------------------------//
    virtual unique_ptr < TYPE > SerializablePtrFactory_make ( const AbstractSerializableTypeInfo& typeInfo ) = 0;

public:

    //----------------------------------------------------------------//
    unique_ptr < TYPE > make ( const AbstractSerializableTypeInfo& typeInfo ) {
        return this->SerializablePtrFactory_make ( typeInfo );
    }
};

//================================================================//
// SerializablePtrFactory
//================================================================//
template < typename TYPE >
class SerializablePtrFactory :
    public AbstractSerializablePtrFactory < TYPE > {
protected:

    //----------------------------------------------------------------//
    virtual unique_ptr < TYPE > SerializablePtrFactory_make ( const AbstractSerializableTypeInfo& typeInfo ) {
        return make_unique < TYPE >();
    }
};

//================================================================//
// SerializableSharedPtr
//================================================================//
template < typename TYPE, typename FACTORY = SerializablePtrFactory < TYPE >>
class SerializableSharedPtr :
    public AbstractSerializablePointer,
    public shared_ptr < TYPE > {
public:

    //----------------------------------------------------------------//
    AbstractSerializable* AbstractSerializablePointer_get () {
        return this->get ();
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializablePointer_make ( const AbstractSerializableTypeInfo& typeInfo ) {
        FACTORY factory;
        *( shared_ptr < TYPE >* )this = factory.make ( typeInfo );
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

//================================================================//
// SerializableUniquePtr
//================================================================//
template < typename TYPE, typename FACTORY = SerializablePtrFactory < TYPE >>
class SerializableUniquePtr :
    public AbstractSerializablePointer,
    public unique_ptr < TYPE > {
public:

    //----------------------------------------------------------------//
    AbstractSerializable* AbstractSerializablePointer_get () {
        return this->get ();
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializablePointer_make ( const AbstractSerializableTypeInfo& typeInfo ) {
        FACTORY factory;
        *( unique_ptr < TYPE >* )this = factory.make ( typeInfo );
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
