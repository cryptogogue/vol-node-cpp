// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/AbstractBlockTree.h>
#include <volition/Block.h>

namespace Volition {

//================================================================//
// BlockTreeTag
//================================================================//

//----------------------------------------------------------------//
BlockTreeCursor BlockTreeTag::operator * () const {

    return this->getCursor ();
}

//----------------------------------------------------------------//
BlockTreeTag::BlockTreeTag () :
    mTree ( NULL ) {
}

//----------------------------------------------------------------//
BlockTreeTag::BlockTreeTag ( string name ) :
    mTree ( NULL ),
    mName ( name ) {
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
bool BlockTreeTag::check ( const AbstractBlockTree* tree ) const {

    return (( this->mName.size () > 0 ) && (( this->mTree == NULL) || ( this->mTree == tree )));
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
bool BlockTreeTag::hasTree () const {

    return ( this->mTree != NULL );
}

//----------------------------------------------------------------//
bool BlockTreeTag::hasName () const {

    return ( this->mName.size () > 0 );
}

//----------------------------------------------------------------//
void BlockTreeTag::reset () {

    this->mTree = NULL;
}

} // namespace Volition
