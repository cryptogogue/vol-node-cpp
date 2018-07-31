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
const void* AbstractValueStack::getRaw ( size_t version ) const {
    return this->AbstractValueStack_getRaw ( version );
}

//----------------------------------------------------------------//
bool AbstractValueStack::isEmpty () const {
    return this->AbstractValueStack_isEmpty ();
}

//----------------------------------------------------------------//
unique_ptr < AbstractValueStack > AbstractValueStack::makeEmptyCopy () const {
    return this->AbstractValueStack_makeEmptyCopy ();
}

//----------------------------------------------------------------//
void AbstractValueStack::pop () {
    this->AbstractValueStack_pop ();
}

//----------------------------------------------------------------//
void AbstractValueStack::pushBackRaw ( const void* value, size_t version ) {
    this->AbstractValueStack_pushBackRaw ( value, version );
}

//----------------------------------------------------------------//
//void AbstractValueStack::pushFrontRaw ( const void* value ) {
//    this->AbstractValueStack_pushFrontRaw ( value );
//}

} // namespace Volition
