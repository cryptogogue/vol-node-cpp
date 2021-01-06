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

    tag.check ( this );
    tag.mTree = this;

    if ( !header ) return BlockTreeCursor ();

    if ( block ) {
        assert ( header->equals ( *block ));
    }
    
    BlockTreeCursor cursor = this->AbstractBlockTree_affirm ( tag, header, block, isProvisional );
    assert ( cursor.hasHeader ());
    
    return cursor;
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
bool AbstractBlockTree::checkStatusTransition ( kBlockTreeEntryStatus from, kBlockTreeEntryStatus to ) {

    if ( from != true ) {

        switch ( from ) {
            
            case STATUS_NEW:
                // --> missing
                // --> complete
                assert ( to != STATUS_INVALID );
                break;
            
            case STATUS_COMPLETE:
                // --> invalid
                assert ( to != STATUS_NEW );
                assert ( to != STATUS_MISSING );
                break;
            
            case STATUS_MISSING:
                // --> complete
                assert ( to != STATUS_NEW );
                assert ( to != STATUS_INVALID );
                break;
                
            case STATUS_INVALID:
                assert ( false ); // no valid transition
                break;
        }
    }
    return true;
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

    assert ( tag.check ( this ));

    if ( tag.getTree ()) {
        return this->AbstractBlockTree_findCursorForTag ( tag );
    }
    return BlockTreeCursor ();
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
BlockTreeCursor AbstractBlockTree::tag ( BlockTreeTag& tag, const BlockTreeCursor& cursor ) {

    assert ( tag.check ( this ));
    
    if ( cursor.getTree ()) {
    
        assert ( cursor.getTree () == this );
        tag.mTree = this;
        
        return this->AbstractBlockTree_tag ( tag, cursor );
    }
    return BlockTreeCursor ();
}

//----------------------------------------------------------------//
BlockTreeCursor AbstractBlockTree::tag ( BlockTreeTag& tag, const BlockTreeTag& otherTag ) {

    assert ( tag.check ( this ));
    
    if ( otherTag.getTree ()) {
    
        assert ( otherTag.check ( this ));
        tag.mTree = this;
        
        return this->AbstractBlockTree_tag ( tag, otherTag );
    }
    return BlockTreeCursor ();
}

//----------------------------------------------------------------//
void AbstractBlockTree::update ( shared_ptr < const Block > block ) {

    this->AbstractBlockTree_update ( block );
}

} // namespace Volition
