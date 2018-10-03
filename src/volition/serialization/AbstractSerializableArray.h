// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SERIALIZATION_ABSTRACTSERIALIZABLEARRAY_H
#define VOLITION_SERIALIZATION_ABSTRACTSERIALIZABLEARRAY_H

#include <volition/common.h>
#include <volition/serialization/AbstractSerializable.h>

namespace Volition {

//================================================================//
// AbstractSerializableArray
//================================================================//
class AbstractSerializableArray :
    public AbstractSerializable {
public:

    //----------------------------------------------------------------//
    virtual void        AbstractSerializableArray_resize        ( size_t size ) = 0;
};

} // namespace Volition
#endif
