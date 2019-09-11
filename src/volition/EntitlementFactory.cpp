// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/BooleanEntitlement.h>
#include <volition/FNV1a.h>
#include <volition/EntitlementFactory.h>
#include <volition/NumericEntitlement.h>
#include <volition/PathEntitlement.h>

namespace Volition {

//================================================================//
// EntitlementFactory
//================================================================//

//----------------------------------------------------------------//
unique_ptr < AbstractEntitlement > EntitlementFactory::makeEntitlement ( string type ) {

    switch ( FNV1a::hash_64 ( type.c_str ())) {
        case FNV1a::const_hash_64 (AbstractEntitlement::ENTITLEMENT_TYPE_BOOLEAN ):     return make_unique < BooleanEntitlement >();
        case FNV1a::const_hash_64 (AbstractEntitlement::ENTITLEMENT_TYPE_NUMERIC ):     return make_unique < NumericEntitlement >();
        case FNV1a::const_hash_64 (AbstractEntitlement::ENTITLEMENT_TYPE_PATH ):        return make_unique < PathEntitlement >();
    }
    return NULL;
}

} // namespace Volition
