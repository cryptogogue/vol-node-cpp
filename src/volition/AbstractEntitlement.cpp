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
shared_ptr < AbstractEntitlement > AbstractEntitlement::clone () const {

    return this->AbstractEntitlement_clone ();
}

//----------------------------------------------------------------//
AbstractEntitlement::Leniency AbstractEntitlement::compare ( const AbstractEntitlement* abstractOther ) const {

    return this->AbstractEntitlement_compare ( abstractOther );
}

//----------------------------------------------------------------//
AbstractEntitlement::Leniency AbstractEntitlement::compareRanges ( Range r0, Range r1 ) {

        // r1: ALWAYS_FALSE         r1: ALWAYS_TRUE             r1 :MAY_BE_TRUE_OR_FALSE
    static const Leniency m [ 3 ][ 3 ] = {
        { IS_EQUALLY_RESTRICTIVE,   IS_MORE_RESTRICTIVE,        IS_MORE_RESTRICTIVE }, // r0: ALWAYS_FALSE
        { IS_LESS_RESTRICTIVE,      IS_EQUALLY_RESTRICTIVE,     IS_LESS_RESTRICTIVE }, // r0: ALWAYS_TRUE
        { IS_LESS_RESTRICTIVE,      IS_MORE_RESTRICTIVE,        IS_LESS_RESTRICTIVE }, // r0: MAY_BE_TRUE_OR_FALSE
    };
    return m [ r0 ][ r1 ];
}

//----------------------------------------------------------------//
AbstractEntitlement::Leniency AbstractEntitlement::compareRanges ( const AbstractEntitlement* e0, const AbstractEntitlement* e1 ) {

    return AbstractEntitlement::compareRanges (
        e0 ? e0->getRange () : ALWAYS_FALSE,
        e1 ? e1->getRange () : ALWAYS_FALSE
    );
}

//----------------------------------------------------------------//
AbstractEntitlement::Range AbstractEntitlement::getRange () const {
    return this->AbstractEntitlement_getRange ();
}

//----------------------------------------------------------------//
bool AbstractEntitlement::isMatchOrSubsetOf ( const AbstractEntitlement* abstractOther ) const {
    Leniency compare = this->compare ( abstractOther );
    return ( compare != IS_LESS_RESTRICTIVE );
}

//----------------------------------------------------------------//
void AbstractEntitlement::print () const {
    this->AbstractEntitlement_print ();
}

//================================================================//
// virtual
//================================================================//

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
