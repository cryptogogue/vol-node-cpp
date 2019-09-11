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
bool AbstractEntitlement::check ( string path ) const {

   const AbstractEntitlement* leaf = this->resolvePath ( path );
   return leaf ? leaf->AbstractEntitlement_check () : false;
}

//----------------------------------------------------------------//
const AbstractEntitlement* AbstractEntitlement::getChild ( string name ) const {

    return this->AbstractEntitlement_getChild ( name );
}

//----------------------------------------------------------------//
bool AbstractEntitlement::isMatchOrSubsetOf ( const AbstractEntitlement* abstractOther ) const {
    return this->AbstractEntitlement_isMatchOrSubsetOf ( abstractOther );
}

//----------------------------------------------------------------//
const AbstractEntitlement* AbstractEntitlement::resolvePath ( string path ) const {
    
    size_t length = path.size ();
    if ( length ) {
    
        size_t delim = 0;
        for ( ; ( delim < length ) && ( path [ delim ] != '.' ); ++delim ) {
        }
        
        if ( delim > 0 ) {
            string name = path.substr ( 0, delim );
            const AbstractEntitlement* child = this->AbstractEntitlement_getChild ( name );
            
            return ( child && ( delim < length )) ? child->resolvePath ( path.substr ( delim + 1, string::npos )) : child;
        }
    }
    return this;
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

//----------------------------------------------------------------//
const AbstractEntitlement* AbstractEntitlement::AbstractEntitlement_getChild ( string name ) const {
    UNUSED ( name );
    return NULL;
}

} // namespace Volition
