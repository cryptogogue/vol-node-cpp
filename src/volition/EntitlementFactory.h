// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_ENTITLEMENTFACTORY_H
#define VOLITION_ENTITLEMENTFACTORY_H

#include <volition/common.h>
#include <volition/serialization/Serialization.h>

namespace Volition {

class AbstractEntitlement;

//================================================================//
// EntitlementFactory
//================================================================//
class EntitlementFactory :
    public AbstractSerializablePtrFactory < AbstractEntitlement > {
private:

    //----------------------------------------------------------------//
    static unique_ptr < AbstractEntitlement >       makeEntitlement     ( string type );

public:

    //----------------------------------------------------------------//
    unique_ptr < AbstractEntitlement > SerializablePtrFactory_make ( const AbstractSerializerFrom& serializer ) override {
        string type = serializer.serializeIn < string >( "type", "" );
        return EntitlementFactory::makeEntitlement ( type );
    }
};

} // namespace Volition
#endif
