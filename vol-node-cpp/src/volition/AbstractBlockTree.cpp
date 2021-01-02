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
kBlockTreeAppendResult AbstractBlockTree::checkAppend ( const BlockHeader& header ) const {

    return this->AbstractBlockTree_checkAppend ( header );
}

//----------------------------------------------------------------//
int AbstractBlockTree::compare ( const BlockTreeCursor& cursor0, const BlockTreeCursor& cursor1, kRewriteMode rewriteMode ) const {

    return this->AbstractBlockTree_compare ( cursor0, cursor1, rewriteMode );
}

//----------------------------------------------------------------//
BlockTreeCursor AbstractBlockTree::findCursorForHash ( string hash ) const {

    return this->AbstractBlockTree_findCursorForHash ( hash );
}

//----------------------------------------------------------------//
BlockTreeCursor AbstractBlockTree::findCursorForTag ( const BlockTreeTag& tag ) const {

    return this->AbstractBlockTree_findCursorForTag ( tag );
}

//----------------------------------------------------------------//
BlockTreeCursor AbstractBlockTree::findRoot ( const BlockTreeCursor& cursor0, const BlockTreeCursor& cursor1 ) const {

    return this->AbstractBlockTree_findRoot ( cursor0, cursor1 );
}

//----------------------------------------------------------------//
BlockTreeCursor AbstractBlockTree::getParent ( const BlockTreeCursor& cursor ) const {

    if ( cursor.getHeight () > 0 ) {
        return this->findCursorForHash ( cursor.getPrevDigest ().toHex ());
    }
    return BlockTreeCursor ();
}

//----------------------------------------------------------------//
BlockTreeCursor AbstractBlockTree::makeCursor ( shared_ptr < const BlockHeader > header, shared_ptr < const Block > block, kBlockTreeEntryStatus status, kBlockTreeEntryMeta meta ) const {

    BlockTreeCursor cursor;
    
    cursor.mTree    = this;
    cursor.mHeader  = header;
    cursor.mBlock   = block;
    cursor.mStatus  = status;
    cursor.mMeta    = meta;
    
    return cursor;
}

//----------------------------------------------------------------//
void AbstractBlockTree::mark ( const BlockTreeCursor& cursor, kBlockTreeEntryStatus status ) {

    this->AbstractBlockTree_mark ( cursor, status );
}

//----------------------------------------------------------------//
void AbstractBlockTree::setTagTree ( BlockTreeTag& tag ) {

    tag.mTree = this;
}

//----------------------------------------------------------------//
BlockTreeCursor AbstractBlockTree::tag ( BlockTreeTag& tag, const BlockTreeCursor& cursor ) {

    return this->AbstractBlockTree_tag ( tag, cursor );
}

//----------------------------------------------------------------//
BlockTreeCursor AbstractBlockTree::tag ( BlockTreeTag& tag, const BlockTreeTag& otherTag ) {

   return this->AbstractBlockTree_tag ( tag, otherTag );
}

//----------------------------------------------------------------//
BlockTreeCursor AbstractBlockTree::update ( shared_ptr < const Block > block ) {

    return this->AbstractBlockTree_update ( block );
}

} // namespace Volition
