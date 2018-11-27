// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SERIALIZATION_ABSTRACTSERIALIZABLECOLLECTION_H
#define VOLITION_SERIALIZATION_ABSTRACTSERIALIZABLECOLLECTION_H

#include <volition/common.h>
#include <volition/serialization/AbstractSerializable.h>

namespace Volition {

//================================================================//
// AbstractSerializableCollection
//================================================================//
class AbstractSerializableCollection :
    public AbstractSerializable {
public:

    //----------------------------------------------------------------//
    virtual void        AbstractSerializableCollection_resize       ( size_t size ) = 0;
};

} // namespace Volition
#endif
