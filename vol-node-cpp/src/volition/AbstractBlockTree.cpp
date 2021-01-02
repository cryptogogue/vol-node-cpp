// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Block.h>
#include <volition/AbstractBlockTree.h>

namespace Volition {

//================================================================//
// AbstractBlockTree
//================================================================//

//----------------------------------------------------------------//
BlockTreeCursor AbstractBlockTree::affirm ( BlockTreeTag& tag, shared_ptr < const BlockHeader > header, shared_ptr < const Block > block, bool isProvisional ) {

    return this->AbstractBlockTree_affirm ( tag, header, block, isProvisional );
}

//----------------------------------------------------------------//
BlockTreeCursor AbstractBlockTree::affirmBlock ( BlockTreeTag& tag, shared_ptr < const Block > block ) {

    return this->affirm ( tag, block, block );
}

//----------------------------------------------------------------//
BlockTreeCursor AbstractBlockTree::affirmHeader ( BlockTreeTag& tag, shared_ptr < const BlockHeader > header ) {

    return this->affirm ( tag, header, NULL );
}

//----------------------------------------------------------------//
BlockTreeCursor AbstractBlockTree::affirmProvisional ( BlockTreeTag& tag, shared_ptr < const BlockHeader > header ) {

    return this->affirm ( tag, header, NULL, true );
}

//----------------------------------------------------------------//
AbstractBlockTree::AbstractBlockTree () {
}

//----------------------------------------------------------------//
AbstractBlockTree::~AbstractBlockTree () {
}

//----------------------------------------------------------------//
AbstractBlockTree::CanAppend AbstractBlockTree::checkAppend ( const BlockHeader& header ) const {

    return this->AbstractBlockTree_checkAppend ( header );
}

//----------------------------------------------------------------//
int AbstractBlockTree::compare ( const BlockTreeCursor& cursor0, const BlockTreeCursor& cursor1, BlockTreeCursor::RewriteMode rewriteMode ) const {

    return this->AbstractBlockTree_compare ( cursor0, cursor1, rewriteMode );
}

//----------------------------------------------------------------//
BlockTreeCursor AbstractBlockTree::findCursorForHash ( string hash ) const {

    return this->AbstractBlockTree_findCursorForHash ( hash );
}

//----------------------------------------------------------------//
BlockTreeCursor AbstractBlockTree::findCursorForTagName ( string tagName ) const {

    return this->AbstractBlockTree_findCursorForTagName ( tagName );
}

//----------------------------------------------------------------//
BlockTreeCursor AbstractBlockTree::findRoot ( const BlockTreeCursor& cursor0, const BlockTreeCursor& cursor1 ) const {

    return this->AbstractBlockTree_findRoot ( cursor0, cursor1 );
}

//----------------------------------------------------------------//
BlockTreeCursor AbstractBlockTree::getParent ( const BlockTreeCursor& cursor ) const {

    return this->AbstractBlockTree_getParent ( cursor );
}

//----------------------------------------------------------------//
void AbstractBlockTree::mark ( const BlockTreeCursor& cursor, BlockTreeNode::Status status ) {

    this->AbstractBlockTree_mark ( cursor, status );
}

//----------------------------------------------------------------//
BlockTreeCursor AbstractBlockTree::tag ( string tagName, string otherTagName ) {

   return this->AbstractBlockTree_tag ( tagName, otherTagName );
}

//----------------------------------------------------------------//
BlockTreeCursor AbstractBlockTree::tag ( BlockTreeTag& tag, const BlockTreeCursor& cursor ) {

    return this->AbstractBlockTree_tag ( tag, cursor );
}

//----------------------------------------------------------------//
BlockTreeCursor AbstractBlockTree::update ( shared_ptr < const Block > block ) {

    return this->AbstractBlockTree_update ( block );
}

} // namespace Volition
