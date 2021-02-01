// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SERIALIZATION_ABSTRACTSERIALIZABLEPTRFACTORY_H
#define VOLITION_SERIALIZATION_ABSTRACTSERIALIZABLEPTRFACTORY_H

#include <volition/serialization/AbstractSerializerFrom.h>
#include <volition/serialization/AbstractSerializerTo.h>

namespace Volition {

//================================================================//
// AbstractSerializablePtrFactory
//================================================================//
template < typename TYPE >
class AbstractSerializablePtrFactory {
protected:

    //----------------------------------------------------------------//
    virtual unique_ptr < TYPE > SerializablePtrFactory_make ( const AbstractSerializerFrom& serializer ) = 0;

public:

    //----------------------------------------------------------------//
    unique_ptr < TYPE > make ( const AbstractSerializerFrom& serializer ) {
        return this->SerializablePtrFactory_make ( serializer );
    }
};

} // namespace Volition
#endif
