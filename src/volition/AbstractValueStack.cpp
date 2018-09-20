// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/State.h>
#include <volition/AbstractValueStack.h>

namespace Volition {

//================================================================//
// AbstractValueStack
//================================================================//

//----------------------------------------------------------------//
AbstractValueStack::AbstractValueStack () :
    mTypeID ( typeid ( void ).hash_code ()) {
}

//----------------------------------------------------------------//
AbstractValueStack::~AbstractValueStack () {
}

//----------------------------------------------------------------//
void AbstractValueStack::copyFrom ( const AbstractValueStack& from ) {
    this->AbstractValueStack_copyFrom ( from );
}

//----------------------------------------------------------------//
void AbstractValueStack::erase ( size_t version ) {
    this->AbstractValueStack_erase ( version );
}

//----------------------------------------------------------------//
const void* AbstractValueStack::getRaw ( size_t version ) const {
    return this->AbstractValueStack_getRaw ( version );
}

//----------------------------------------------------------------//
unique_ptr < AbstractValueStack > AbstractValueStack::makeEmptyCopy () const {
    return this->AbstractValueStack_makeEmptyCopy ();
}

//----------------------------------------------------------------//
void AbstractValueStack::setRaw ( size_t version, const void* value ) {
    this->AbstractValueStack_setRaw ( version, value );
}

//----------------------------------------------------------------//
size_t AbstractValueStack::size () const {
    return this->AbstractValueStack_size ();
}

} // namespace Volition
