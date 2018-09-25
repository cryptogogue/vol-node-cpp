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
/** \brief Copies a single value from the source stack to the destination stack.

    \param      from        The value stack to copy from.
    \param      version     The version of the value to copy.
*/
void AbstractValueStack::copyValueFrom ( const AbstractValueStack& from, size_t version ) {
    this->AbstractValueStack_copyValueFrom ( from, version );
}

//----------------------------------------------------------------//
/** \brief Erases the value (if any) corresponding to the given version.

    \param      version     The version of the value to erase.
*/
void AbstractValueStack::erase ( size_t version ) {
    this->AbstractValueStack_erase ( version );
}

//----------------------------------------------------------------//
/** \brief Concatenates values from one stack to another. All values in
    the source stack must have versions greater than values in the
    destination stack.
 
    \param      to          The stack to copy to.
*/
void AbstractValueStack::join ( AbstractValueStack& to ) const {
    this->AbstractValueStack_join ( to );
}


//----------------------------------------------------------------//
/** \brief Creates an empty copy of the source stack.

    \return     A new AbstractValueStack with the same type implementation as the source.
*/
unique_ptr < AbstractValueStack > AbstractValueStack::makeEmptyCopy () const {
    return this->AbstractValueStack_makeEmptyCopy ();
}

//----------------------------------------------------------------//
/** \brief Returns the size (i.e. total values) held in the stack.

    \return     The size of the stack.
*/
size_t AbstractValueStack::size () const {
    return this->AbstractValueStack_size ();
}

} // namespace Volition
