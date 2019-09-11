// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/AbstractEntitlement.h>

namespace Volition {

//================================================================//
// AbstractEntitlement
//================================================================//

//----------------------------------------------------------------//
AbstractEntitlement::AbstractEntitlement () {
}

//----------------------------------------------------------------//
AbstractEntitlement::~AbstractEntitlement () {
}

//----------------------------------------------------------------//
bool AbstractEntitlement::check () const {

   return this->AbstractEntitlement_check ();
}

//----------------------------------------------------------------//
bool AbstractEntitlement::isMatchOrSubsetOf ( const AbstractEntitlement* abstractOther ) const {
    return this->AbstractEntitlement_isMatchOrSubsetOf ( abstractOther );
}

//----------------------------------------------------------------//
bool AbstractEntitlement::AbstractEntitlement_check () const {
    return false;
}

//----------------------------------------------------------------//
bool AbstractEntitlement::AbstractEntitlement_check ( double value ) const {
    UNUSED ( value );
    return false;
}

} // namespace Volition
