// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Block.h>
#include <volition/BlockTree.h>
#include <volition/BlockTreeNode.h>

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

    if ( other.mBlockTree ) {
        this->mBlockTree = other.mBlockTree;
        other.mBlockTree->tag ( *this, other );
    }
    return *this;
}

//----------------------------------------------------------------//
BlockTreeCursor BlockTreeTag::operator * () const {

    return this->getCursor ();
}

//----------------------------------------------------------------//
BlockTreeTag::BlockTreeTag () :
    mBlockTree ( NULL ) {
}

//----------------------------------------------------------------//
BlockTreeTag::BlockTreeTag ( BlockTreeTag& other ) :
    mTagName ( other.mTagName ),
    mBlockTree ( other.mBlockTree ) {
}

//----------------------------------------------------------------//
BlockTreeTag::~BlockTreeTag () {
}

//----------------------------------------------------------------//
bool BlockTreeTag::equals ( const BlockTreeTag& rhs ) const {

    return ( this->getCursor ().equals ( rhs.getCursor ()));
}

//----------------------------------------------------------------//
BlockTreeCursor BlockTreeTag::getCursor () const {

    return this->mBlockTree ? this->mBlockTree->findCursorForTag ( *this ) : BlockTreeCursor ();
}

//----------------------------------------------------------------//
bool BlockTreeTag::hasCursor () const {

    return this->mBlockTree && ( this->mTagName.size ()) && ( this->mBlockTree->findCursorForTag ( *this ).hasHeader ());
}

//----------------------------------------------------------------//
void BlockTreeTag::setTagName ( string tagName ) {

    this->mTagName = tagName;
}

//================================================================//
// virtual
//================================================================//

//----------------------------------------------------------------//
const BlockHeaderFields& BlockTreeTag::HasBlockHeader_getFields () const {

    return this->getCursor ().getFields ();
}

} // namespace Volition
