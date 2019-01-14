// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SERIALIZATION_SERIALIZABLEPTRFACTORY_H
#define VOLITION_SERIALIZATION_SERIALIZABLEPTRFACTORY_H

#include <volition/serialization/AbstractSerializablePtrFactory.h>

namespace Volition {

//================================================================//
// SerializablePtrFactory
//================================================================//
template < typename TYPE >
class SerializablePtrFactory :
    public AbstractSerializablePtrFactory < TYPE > {
protected:

    //----------------------------------------------------------------//
    virtual unique_ptr < TYPE > SerializablePtrFactory_make ( const AbstractSerializerFrom& serializer ) {
        return make_unique < TYPE >();
    }
};

} // namespace Volition
#endif
