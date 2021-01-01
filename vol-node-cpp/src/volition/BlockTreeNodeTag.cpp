// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Block.h>
#include <volition/BlockTree.h>
#include <volition/BlockTreeNode.h>

namespace Volition {

//================================================================//
// BlockTreeNodeTag
//================================================================//

//----------------------------------------------------------------//
BlockTreeNodeTag& BlockTreeNodeTag::operator = ( BlockTreeCursor other ) {

    if ( other.hasHeader ()) {
        assert ( other.mTree );
        assert ( !this->mBlockTree || ( this->mBlockTree == other.mTree ));
        other.mTree->tag ( *this, other );
    }
    else {
        this->mBlockTree = NULL;
    }
    return *this;
}

//----------------------------------------------------------------//
BlockTreeNodeTag& BlockTreeNodeTag::operator = ( const BlockTreeNodeTag& other ) {

    assert ( this->mTagName.size () > 0 );
    if ( other.mBlockTree ) {
        this->mBlockTree = other.mBlockTree;
        this->mBlockTree->tag ( this->mTagName, other.mTagName );
    }
    return *this;
}

//----------------------------------------------------------------//
BlockTreeCursor BlockTreeNodeTag::operator * () const {

    return this->getCursor ();
}

//----------------------------------------------------------------//
BlockTreeNodeTag::BlockTreeNodeTag () :
    mBlockTree ( NULL ) {
}

//----------------------------------------------------------------//
BlockTreeNodeTag::BlockTreeNodeTag ( BlockTreeNodeTag& other ) :
    mTagName ( other.mTagName ),
    mBlockTree ( other.mBlockTree ) {
}

//----------------------------------------------------------------//
BlockTreeNodeTag::~BlockTreeNodeTag () {
}

//----------------------------------------------------------------//
bool BlockTreeNodeTag::equals ( const BlockTreeNodeTag& rhs ) const {

    return ( this->getCursor ().equals ( rhs.getCursor ()));
}

//----------------------------------------------------------------//
BlockTreeCursor BlockTreeNodeTag::getCursor () const {

    return this->mBlockTree ? this->mBlockTree->findCursorForTagName ( this->mTagName ) : BlockTreeCursor ();
}

//----------------------------------------------------------------//
bool BlockTreeNodeTag::hasCursor () const {

    return this->mBlockTree && ( this->mTagName.size ()) && ( this->mBlockTree->findCursorForTagName ( this->mTagName ).hasHeader ());
}

//----------------------------------------------------------------//
void BlockTreeNodeTag::setTagName ( string tagName ) {

    this->mTagName = tagName;
}

//================================================================//
// virtual
//================================================================//

//----------------------------------------------------------------//
const BlockHeaderFields& BlockTreeNodeTag::HasBlockHeader_getFields () const {

    return this->getCursor ().getFields ();
}

} // namespace Volition
