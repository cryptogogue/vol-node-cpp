// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/AbstractBlockTree.h>
#include <volition/Block.h>

namespace Volition {

//================================================================//
// BlockTreeTag
//================================================================//

//----------------------------------------------------------------//
BlockTreeTag& BlockTreeTag::operator = ( BlockTreeCursor other ) {

    if ( other.mTree ) {
        other.mTree->tag ( *this, other );
    }
    return *this;
}

//----------------------------------------------------------------//
BlockTreeTag& BlockTreeTag::operator = ( const BlockTreeTag& other ) {

    if ( other.mTree ) {
        this->mTree = other.mTree;
        other.mTree->tag ( *this, other );
    }
    return *this;
}

//----------------------------------------------------------------//
BlockTreeCursor BlockTreeTag::operator * () const {

    return this->getCursor ();
}

//----------------------------------------------------------------//
BlockTreeTag::BlockTreeTag () :
    mTree ( NULL ) {
}

//----------------------------------------------------------------//
BlockTreeTag::BlockTreeTag ( BlockTreeTag& other ) :
    mTree ( other.mTree ),
    mName ( other.mName ) {
}

//----------------------------------------------------------------//
BlockTreeTag::~BlockTreeTag () {
}

//----------------------------------------------------------------//
bool BlockTreeTag::checkTree ( const AbstractBlockTree* tree ) const {

    return (( this->mTree == NULL) || ( this->mTree == tree ));
}

//----------------------------------------------------------------//
bool BlockTreeTag::equals ( const BlockTreeTag& rhs ) const {

    return ( this->getCursor ().equals ( rhs.getCursor ()));
}

//----------------------------------------------------------------//
BlockTreeCursor BlockTreeTag::getCursor () const {

    return this->mTree ? this->mTree->findCursorForTag ( *this ) : BlockTreeCursor ();
}

//----------------------------------------------------------------//
bool BlockTreeTag::hasCursor () const {

    return this->mTree && ( this->mName.size ()) && ( this->mTree->findCursorForTag ( *this ).hasHeader ());
}

//----------------------------------------------------------------//
bool BlockTreeTag::hasName () const {

    return ( this->mName.size () > 0 );
}

//================================================================//
// virtual
//================================================================//

//----------------------------------------------------------------//
const BlockHeaderFields& BlockTreeTag::HasBlockHeader_getFields () const {

    return this->getCursor ().getFields ();
}

} // namespace Volition
