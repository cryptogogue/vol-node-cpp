// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Block.h>
#include <volition/BlockTree.h>
#include <volition/BlockTreeNode.h>

// To compare chains:
// 1. Find the common root.
// 2. Get the interval between the timestamp of the end of the longest branch and its first block (excluding the root).
//      a. From the interval, divide by the lookback window to calculate COMPARE_COUNT.
// 3. From the common root, up to the COMPARE_COUNT, compare each block and tally the score for each chain.
//      a. +1 for the winner, -1 to the loser; 0 if tied.
// 4. Select the winner.
//      a. The chain with the highest score wins.
//      b. If chains are tied and the same length, pick the chain with the best ending block.
//      c. If chains are tied and different length, extend the shorter chain by one as a tie-breaker.

namespace Volition {

//================================================================//
// BlockTreeSegment
//================================================================//

//----------------------------------------------------------------//
size_t BlockTreeSegment::getFullLength () const {

    return this->mHead ? (( **this->mTop ).getHeight () - ( **this->mHead ).getHeight ()) : 0;
}

//----------------------------------------------------------------//
size_t BlockTreeSegment::getRewriteDefeatCount () const {

    time_t window = ( **this->mHead ).getRewriteWindow (); // TODO: account for different rewrite windows in segment
    return ( size_t )ceil ( difftime (( **this->mTop ).getTime (), ( **this->mHead ).getTime ()) / window );
}

//----------------------------------------------------------------//
size_t BlockTreeSegment::getSegLength () const {

    return this->mHead ? (( **this->mTail ).getHeight () - ( **this->mHead ).getHeight ()) : 0;
}

//================================================================//
// BlockTreeRoot
//================================================================//

//----------------------------------------------------------------//
size_t BlockTreeRoot::getSegLength () const {

    return this->mSeg0.getSegLength ();
}

//================================================================//
// BlockTreeNode
//================================================================//

//----------------------------------------------------------------//
BlockTreeNode::BlockTreeNode () :
    mTree ( NULL ),
    mStatus ( STATUS_INVALID ),
    mMeta ( META_NONE ) {
}

//----------------------------------------------------------------//
BlockTreeNode::~BlockTreeNode () {

    if ( this->mTree && this->mHeader ) {
        this->mTree->mNodes.erase ( this->mHeader->getDigest ());
    }
    this->clearParent ();
}

//----------------------------------------------------------------//
bool BlockTreeNode::checkStatus ( Status status ) const {

    return ( this->mStatus & status );
}

//----------------------------------------------------------------//
void BlockTreeNode::clearParent () {

    if ( this->mParent ) {
        this->mParent->mChildren.erase ( this );
    }
    this->mParent = NULL;
}

//----------------------------------------------------------------//
int BlockTreeNode::compare ( shared_ptr < const BlockTreeNode > node0, shared_ptr < const BlockTreeNode > node1, RewriteMode rewriteMode ) {

    assert ( node0 && node1 );

    BlockTreeRoot root = BlockTreeNode::findRoot ( node0, node1);

    size_t fullLength0  = root.mSeg0.getFullLength ();
    size_t fullLength1  = root.mSeg1.getFullLength ();

    if ( rewriteMode == REWRITE_WINDOW ) {
        
        size_t segLength = root.getSegLength (); // length of the shorter segment (if different lengths)
        
        // if one chain is shorter, it must have enough blocks to "defeat" the longer chain (as a function of time)
        if (( segLength < fullLength0 ) && ( segLength < root.mSeg0.getRewriteDefeatCount ())) return -1;
        if (( segLength < fullLength1 ) && ( segLength < root.mSeg1.getRewriteDefeatCount ())) return 1;
    }

    int score = 0;

    shared_ptr < const BlockTreeNode > cursor0 = root.mSeg0.mTail;
    shared_ptr < const BlockTreeNode > cursor1 = root.mSeg1.mTail;

    while ( cursor0 != cursor1 ) {
    
        score += BlockHeader::compare ( *cursor0->mHeader, *cursor1->mHeader );
        
        cursor0 = cursor0->mParent;
        cursor1 = cursor1->mParent;
    }

    if (( score == 0 ) && ( fullLength0 != fullLength1 )) {
        return ( fullLength0 < fullLength1 ) ? 1 : -1;
    }
    return score < 0 ? -1 : score > 0 ? 1 : 0;
}

//----------------------------------------------------------------//
BlockTreeNode::ConstPtr BlockTreeNode::findFirstIncomplete () const {

    if ( this->mStatus & ( STATUS_INVALID | STATUS_COMPLETE )) return NULL;
    if ( this->mParent && ( this->mParent->mStatus != STATUS_COMPLETE )) return this->mParent->findFirstIncomplete ();
    
    assert ( this->mStatus & ( STATUS_NEW | STATUS_MISSING ));
    return this->shared_from_this ();
}

//----------------------------------------------------------------//
shared_ptr < const BlockTreeNode > BlockTreeNode::findInsertion ( string minerID, const Digest& visage ) const {

    return this->mParent->findInsertionRecurse ( this->shared_from_this (), minerID, visage );
}

//----------------------------------------------------------------//
shared_ptr < const BlockTreeNode > BlockTreeNode::findInsertionRecurse ( shared_ptr < const BlockTreeNode > tail, string minerID, const Digest& visage ) const {

    if ( this->mHeader->getMinerID () == minerID ) return tail;

    shared_ptr < const BlockHeader > parent = this->mParent ? this->mParent->mHeader : NULL;
    if ( !parent ) return tail;

    Digest charm = parent->getNextCharm ( visage );

    if ( BlockHeader::compare ( charm, this->mHeader->getCharm ()) < 0 ) {
        printf ( "BACKUP!\n" );
        return this->mParent;
    }

    return this->mParent->findInsertionRecurse ( tail, minerID, visage );
}

//----------------------------------------------------------------//
BlockTreeRoot BlockTreeNode::findRoot ( shared_ptr < const BlockTreeNode > node0, shared_ptr < const BlockTreeNode > node1 ) {

    BlockTreeRoot root;

    if ( node0->mTree && ( node0->mTree == node1->mTree )) {
    
        BlockTreeSegment seg0;
        BlockTreeSegment seg1;
    
        seg0.mTop = node0;
        seg1.mTop = node1;
    
        size_t height0 = node0->mHeader->getHeight ();
        size_t height1 = node1->mHeader->getHeight ();

        size_t height = height0 < height1 ? height0 : height1;
        
        while ( height < node0->mHeader->getHeight ()) {
            node0 = node0->mParent;
        }
        
        while ( height < node1->mHeader->getHeight ()) {
            node1 = node1->mParent;
        }
        
        seg0.mTail = node0;
        seg1.mTail = node1;

        while ( node0 != node1 ) {
        
            seg0.mHead = node0;
            seg1.mHead = node1;
            
            node0 = node0->mParent;
            node1 = node1->mParent;
        }
        
        assert ( node0 && node1 );
        
        if ( seg0.mHead ) {
            root.mSeg0 = seg0;
        }
        
        if ( seg1.mHead ) {
            root.mSeg1 = seg1;
        }
        
        root.mRoot = node0;
        
        assert ( root.mSeg0.getSegLength () == root.mSeg1.getSegLength ());
    }
    return root;
}

//----------------------------------------------------------------//
shared_ptr < const Block > BlockTreeNode::getBlock () const {

    return this->mBlock;
}

//----------------------------------------------------------------//
shared_ptr < const BlockHeader > BlockTreeNode::getBlockHeader () const {

    return this->mHeader;
}

//----------------------------------------------------------------//
shared_ptr < const BlockTreeNode > BlockTreeNode::getParent () const {

    return this->mParent;
}

//----------------------------------------------------------------//
BlockTreeNode::Status BlockTreeNode::getStatus () const {
    
    return this->mStatus;
}

//----------------------------------------------------------------//
bool BlockTreeNode::isAncestorOf ( ConstPtr tail ) const {

    assert ( this->mHeader );
    assert ( tail->mHeader );
    
    while (( **tail ).getHeight () > ( **this ).getHeight ()) {
        tail = tail->getParent ();
    }
    return ( **this == **tail );
}

//----------------------------------------------------------------//
bool BlockTreeNode::isComplete () const {

    return ( this->mStatus & STATUS_COMPLETE );
}

//----------------------------------------------------------------//
bool BlockTreeNode::isInvalid () const {

    return ( this->mStatus & STATUS_INVALID );
}

//----------------------------------------------------------------//
bool BlockTreeNode::isMissing () const {

    return ( this->mStatus & STATUS_MISSING );
}

//----------------------------------------------------------------//
bool BlockTreeNode::isMissingOrInvalid () const {

    return ( this->mStatus & ( STATUS_MISSING | STATUS_INVALID ));
}

//----------------------------------------------------------------//
bool BlockTreeNode::isNew () const {

    return ( this->mStatus & STATUS_NEW );
}

//----------------------------------------------------------------//
bool BlockTreeNode::isRefused () const {

    return ( this->mMeta == META_REFUSED );
}

//----------------------------------------------------------------//
void BlockTreeNode::logBranchRecurse ( string& str ) const {

    if ( this->mParent ) {
        this->mParent->logBranchRecurse ( str );
    }
    const BlockHeader& header = *this->mHeader;
    
    cc8* status = "";
    
    if ( this->mMeta != META_NONE ) {
    
        switch ( this->mMeta ) {
            
            case META_PROVISIONAL:
                status = "*";
                break;
            
            case META_REFUSED:
                status = "#";
                break;
            
            case META_ROOT:
                status = "-";
                break;
            
            default:
                break;
        }
    }
    else {
    
        switch ( this->mStatus ) {
            
            case STATUS_NEW:
                status = "N";
                break;
                
            case STATUS_COMPLETE:
                status = "C";
                break;
            
            case STATUS_MISSING:
                status = "?";
                break;
                
            case STATUS_INVALID:
                status = "X";
                break;
        }
    }
    
    string charm = this->writeCharmTag ();
    cc8* format = this->mBlock ? "%s%d [%s:%s:%s]" : "%s%d <%s:%s:%s>";
    
    size_t height = header.getHeight ();
    Format::write ( str, format, this->mParent ? ", " : "", ( int )height, ( height > 0 ) ? header.getMinerID ().c_str () : "-", charm.c_str (), status );
}

//----------------------------------------------------------------//
void BlockTreeNode::mark ( BlockTreeNode::Status status ) {

    if ( status == this->mStatus ) return;

    switch ( this->mStatus ) {
        
        case STATUS_NEW:
            // --> missing
            // --> complete
            assert ( status != STATUS_INVALID );
            break;
        
        case STATUS_COMPLETE:
            // --> invalid
            assert ( status != STATUS_NEW );
            assert ( status != STATUS_MISSING );
            break;
        
        case STATUS_MISSING:
            // --> complete
            assert ( status != STATUS_NEW );
            assert ( status != STATUS_INVALID );
            break;
            
        case STATUS_INVALID:
            assert ( false ); // no valid transition
            break;
    }
    
    this->mStatus = status;
    
    set < BlockTreeNode* >::iterator childIt = this->mChildren.begin ();
    for ( ; childIt != this->mChildren.end (); ++childIt ) {
        ( *childIt )->mark ( status );
    }
}

//----------------------------------------------------------------//
void BlockTreeNode::markComplete () {
    
    if ( !this->mBlock ) return;
    if ( this->mParent && ( this->mParent->mStatus != STATUS_COMPLETE )) return;
    
    this->mStatus = STATUS_COMPLETE;
    
    set < BlockTreeNode* >::iterator childIt = this->mChildren.begin ();
    for ( ; childIt != this->mChildren.end (); ++childIt ) {
        ( *childIt )->markComplete ();
    }
}

//----------------------------------------------------------------//
void BlockTreeNode::markRefused () {

    this->mMeta = META_REFUSED;
    
    while ( this->mChildren.size ()) {
        set < BlockTreeNode* >::iterator childIt = this->mChildren.begin ();
        ( *childIt )->markRefused ();
    }
    
    this->clearParent ();
    this->mTree->mNodes.erase ( this->mHeader->getDigest ());
}

//----------------------------------------------------------------//
BlockTreeNode::ConstPtr BlockTreeNode::trim ( Status status ) const {

    BlockTreeNode::ConstPtr cursor = this->shared_from_this ();

    while ( cursor && ( cursor->mStatus & status )) {
        cursor = cursor->getParent ();
    }
    return cursor;
}

//----------------------------------------------------------------//
BlockTreeNode::ConstPtr BlockTreeNode::trimInvalid () const {

    return this->trim ( STATUS_INVALID );
}

//----------------------------------------------------------------//
BlockTreeNode::ConstPtr BlockTreeNode::trimMissing () const {

    return this->trim ( STATUS_MISSING );
}

//----------------------------------------------------------------//
BlockTreeNode::ConstPtr BlockTreeNode::trimMissingOrInvalid () const {

    return this->trim (( Status )( STATUS_MISSING | STATUS_INVALID ));
}

//----------------------------------------------------------------//
string BlockTreeNode::writeBranch () const {

    string str;
    this->logBranchRecurse ( str );
    return str;
}

//----------------------------------------------------------------//
string BlockTreeNode::writeCharmTag () const {

    return this->mHeader ? this->mHeader->getCharm ().toHex ().substr ( 0, 6 ) : "";
}

} // namespace Volition
