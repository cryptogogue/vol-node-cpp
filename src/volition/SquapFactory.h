// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SQUAPFACTORY_H
#define VOLITION_SQUAPFACTORY_H

#include <volition/common.h>
#include <volition/AbstractSquap.h>
#include <volition/FNV1a.h>

namespace Volition {

//================================================================//
// SquapFactory
//================================================================//
class SquapFactory :
    public AbstractSerializablePtrFactory < AbstractSquap > {
public:

    //----------------------------------------------------------------//
    unique_ptr < AbstractSquap > SerializablePtrFactory_make ( const AbstractSerializerFrom& serializer ) override;
};

} // namespace Volition
#endif
