// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/State.h>
#include <volition/AbstractValueStack.h>

namespace Volition {

//================================================================//
// AbstractValueStack
//================================================================//

//----------------------------------------------------------------//
AbstractValueStack::AbstractValueStack () {
}

//----------------------------------------------------------------//
AbstractValueStack::~AbstractValueStack () {
}

//----------------------------------------------------------------//
void AbstractValueStack::copyFrom ( const AbstractValueStack& from ) {
    this->AbstractValueStack_copyFrom ( from );
}

//----------------------------------------------------------------//
void AbstractValueStack::copyValueFrom ( const AbstractValueStack& from, size_t version ) {
    this->AbstractValueStack_copyValueFrom ( from, version );
}

//----------------------------------------------------------------//
void AbstractValueStack::erase ( size_t version ) {
    this->AbstractValueStack_erase ( version );
}

//----------------------------------------------------------------//
unique_ptr < AbstractValueStack > AbstractValueStack::makeEmptyCopy () const {
    return this->AbstractValueStack_makeEmptyCopy ();
}

//----------------------------------------------------------------//
size_t AbstractValueStack::size () const {
    return this->AbstractValueStack_size ();
}

} // namespace Volition
