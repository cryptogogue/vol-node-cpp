// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Block.h>
#include <volition/AbstractBlockTree.h>

namespace Volition {

//================================================================//
// BlockTreeSegment
//================================================================//

//----------------------------------------------------------------//
size_t BlockTreeSegment::getFullLength ( const Iterator& root ) const {

    return ( this->mTop->getHeight () - root->getHeight ());
}

//----------------------------------------------------------------//
size_t BlockTreeSegment::getRewriteDefeatCount ( const Iterator& root ) const {

    time_t window = this->mHead->getRewriteWindow (); // TODO: account for different rewrite windows in segment
    size_t max = ( size_t )ceil ( difftime ( this->mTop->getTime (), root->getTime ()) / window );
    size_t length = this->getFullLength ( root );
    
    return length < max ? length : max;
}

//----------------------------------------------------------------//
BlockTreeSegment::Iterator BlockTreeSegment::pushFront ( const BlockTreeCursor& cursor ) {

    this->push_front ( cursor );
    return this->cbegin ();
}

//================================================================//
// BlockTreeFork
//================================================================//

//----------------------------------------------------------------//
size_t BlockTreeFork::getSegLength () const {

    return this->mSeg0.mTail->getHeight () - this->mRoot->getHeight ();
}

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

    string hash = header.getDigest ().toHex ();

    BlockTreeCursor cursor = this->findCursorForHash ( hash );
    if ( cursor.hasHeader ()) return ALREADY_EXISTS;

    BlockTreeCursor prevCursor = this->findCursorForHash ( header.getPrevDigest ());

    if ( !prevCursor.hasHeader ()) return MISSING_PARENT;
    if ( header.getTime () < prevCursor.getNextTime ()) return TOO_SOON;

    return APPEND_OK;
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

    BlockTreeFork fork;
    this->findFork ( fork, cursor0, cursor1 );

    size_t fullLength0  = fork.mSeg0.getFullLength ( fork.mRoot );
    size_t fullLength1  = fork.mSeg1.getFullLength ( fork.mRoot );
    size_t segLength    = fork.getSegLength (); // length of the comparison segment

    if ( rewriteMode == kRewriteMode::REWRITE_WINDOW ) {
        
        // if the segment is shorter, it must have enough blocks to "defeat" the longer chain (as a function of time)
        if (( segLength < fullLength0 ) && ( segLength < fork.mSeg0.getRewriteDefeatCount ( fork.mRoot ))) return -1;
        if (( segLength < fullLength1 ) && ( segLength < fork.mSeg1.getRewriteDefeatCount ( fork.mRoot ))) return 1;
    }

    BlockTreeSegment::Iterator cursorIt0 = fork.mSeg0.mTail;
    BlockTreeSegment::Iterator cursorIt1 = fork.mSeg1.mTail;

    int tieBreaker = 0;
    int score = 0;

    for ( size_t i = 0; i < segLength; ++i ) {
    
        const BlockTreeCursor& cusor0 = *cursorIt0;
        const BlockTreeCursor& cusor1 = *cursorIt1;
    
        assert ( cusor0.getDigest ());
        assert ( cusor1.getDigest ());
        assert ( cusor0.getDigest () != cusor1.getDigest ());
    
        tieBreaker = BlockHeader::compare ( *cursorIt0->mHeader, *cursorIt1->mHeader );
        score += tieBreaker;
        
        --cursorIt0;
        --cursorIt1;
    }
    
    return score == 0 ? tieBreaker : ( score < 0 ? -1 : 1 );
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
void AbstractBlockTree::findFork ( BlockTreeFork& fork, BlockTreeCursor cursor0, BlockTreeCursor cursor1 ) const {

    assert ( cursor0.mTree && ( cursor0.mTree == cursor1.mTree ));
    
    BlockTreeSegment& seg0 = fork.mSeg0;
    BlockTreeSegment& seg1 = fork.mSeg1;

    seg0.mTop = seg0.pushFront ( cursor0 );
    seg1.mTop = seg1.pushFront ( cursor1 );

    size_t height0 = cursor0.getHeight ();
    size_t height1 = cursor1.getHeight ();

    size_t height = height0 < height1 ? height0 : height1;
    
    while ( cursor0.hasParent () && ( height < cursor0.getHeight ())) {
        cursor0 = cursor0.getParent ();
        seg0.pushFront ( cursor0 );
    }
    
    while ( cursor1.hasParent () && ( height < cursor1.getHeight ())) {
        cursor1 = cursor1.getParent ();
        seg1.pushFront ( cursor1 );
    }

    seg0.mHead = seg0.begin ();
    seg1.mHead = seg1.begin ();

    seg0.mTail = seg0.begin ();
    seg1.mTail = seg1.begin ();

    while ( !cursor0.equals ( cursor1 )) {
    
        seg0.mHead = seg0.begin ();
        seg1.mHead = seg1.begin ();
        
        cursor0 = cursor0.getParent ();
        cursor1 = cursor1.getParent ();
        
        seg0.pushFront ( cursor0 );
        seg1.pushFront ( cursor1 );
    }
    
    assert ( cursor0.hasHeader () && cursor1.hasHeader ());
    
    fork.mRoot = seg0.begin ();
    
    assert ( fork.mSeg0.mTail->getHeight () == fork.mSeg1.mTail->getHeight ());
}

//----------------------------------------------------------------//
BlockTreeCursor AbstractBlockTree::findRoot ( const BlockTreeCursor& cursor0, const BlockTreeCursor& cursor1 ) const {

    BlockTreeFork fork;
    this->findFork ( fork, cursor0, cursor1 );
    return *fork.mRoot;
}

//----------------------------------------------------------------//
shared_ptr < const Block > AbstractBlockTree::getBlock ( const BlockTreeCursor& cursor ) const {

    return this->AbstractBlockTree_getBlock ( cursor );
}

//----------------------------------------------------------------//
BlockTreeCursor AbstractBlockTree::getParent ( const BlockTreeCursor& cursor ) const {

    if ( cursor.getHeight () > 0 ) {
        return this->findCursorForHash ( cursor.getPrevDigest ().toHex ());
    }
    return BlockTreeCursor ();
}

//----------------------------------------------------------------//
BlockTreeCursor AbstractBlockTree::makeCursor ( shared_ptr < const BlockHeader > header, kBlockTreeEntryStatus status, kBlockTreeEntryMeta meta, bool hasBlock ) const {

    BlockTreeCursor cursor;
    
    cursor.mTree        = this;
    cursor.mHeader      = header;
    cursor.mStatus      = status;
    cursor.mMeta        = meta;
    cursor.mHasBlock    = hasBlock;
    
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
