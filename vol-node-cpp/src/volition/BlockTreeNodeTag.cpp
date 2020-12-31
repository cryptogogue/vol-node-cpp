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
BlockTreeNodeTag& BlockTreeNodeTag::operator = ( const BlockTreeNode* other ) {

    if ( other ) {
        assert ( other->mTree );
        assert ( !this->mBlockTree || ( this->mBlockTree == other->mTree ));
        other->mTree->tag ( *this, other );
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
bool BlockTreeNodeTag::operator == ( const BlockTreeNodeTag& other ) const {
    return ( this->get () == other.get ());
}

//----------------------------------------------------------------//
const BlockTreeNode& BlockTreeNodeTag::operator * () const {

    const BlockTreeNode* node = this->get ();
    assert ( node );
    return *node;
}

//----------------------------------------------------------------//
const BlockTreeNode* BlockTreeNodeTag::operator -> () const {

    return this->get ();
}

//----------------------------------------------------------------//
BlockTreeNodeTag::operator bool () const {

    return this->mBlockTree && ( this->mTagName.size ()) && ( this->mBlockTree->findNodeForTagName ( this->mTagName ) != NULL );
}

//----------------------------------------------------------------//
BlockTreeNodeTag::operator const BlockTreeNode* () const {

    return this->get ();
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
const BlockTreeNode* BlockTreeNodeTag::get () const {

    const BlockTreeNode* node = this->mBlockTree ? this->mBlockTree->findNodeForTagName ( this->mTagName ) : NULL;
    return node;
}

//----------------------------------------------------------------//
void BlockTreeNodeTag::setTagName ( string tagName ) {

    this->mTagName = tagName;
}

} // namespace Volition
