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

    return ( **this->mTop ).getHeight () - ( **this->mHead ).getHeight ();
}

//----------------------------------------------------------------//
size_t BlockTreeSegment::getRewriteDefeatCount () const {

    time_t window = ( **this->mHead ).getRewriteWindow (); // TODO: account for different rewrite windows in segment
    return ( size_t )ceil ( difftime (( **this->mTop ).getTime (), ( **this->mHead ).getTime ()) / window );
}

//----------------------------------------------------------------//
size_t BlockTreeSegment::getSegLength () const {

    return ( **this->mTail ).getHeight () - ( **this->mHead ).getHeight ();
}

//================================================================//
// BlockTreeRoot
//================================================================//

//----------------------------------------------------------------//
size_t BlockTreeFork::getSegLength () const {

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
int BlockTreeNode::compare ( const BlockTreeNode* node0, const BlockTreeNode* node1, RewriteMode rewriteMode ) {

    assert ( node0 && node1 );

    BlockTreeFork root = BlockTreeNode::findFork ( node0, node1);

    size_t fullLength0  = root.mSeg0.getFullLength ();
    size_t fullLength1  = root.mSeg1.getFullLength ();

    if ( rewriteMode == REWRITE_WINDOW ) {
        
        size_t segLength = root.getSegLength (); // length of the shorter segment (if different lengths)
        
        // if one chain is shorter, it must have enough blocks to "defeat" the longer chain (as a function of time)
        if (( segLength < fullLength0 ) && ( segLength < root.mSeg0.getRewriteDefeatCount ())) return -1;
        if (( segLength < fullLength1 ) && ( segLength < root.mSeg1.getRewriteDefeatCount ())) return 1;
    }

    int score = 0;

    const BlockTreeNode* cursor0 = root.mSeg0.mTail;
    const BlockTreeNode* cursor1 = root.mSeg1.mTail;

    while ( cursor0 != cursor1 ) {
    
        score += BlockHeader::compare ( *cursor0->mHeader, *cursor1->mHeader );
        
        cursor0 = cursor0->mParent.get ();
        cursor1 = cursor1->mParent.get ();
    }

    if (( score == 0 ) && ( fullLength0 != fullLength1 )) {
        return ( fullLength0 < fullLength1 ) ? 1 : -1;
    }
    return score < 0 ? -1 : score > 0 ? 1 : 0;
}

//----------------------------------------------------------------//
BlockTreeFork BlockTreeNode::findFork ( const BlockTreeNode* node0, const BlockTreeNode* node1 ) {

    BlockTreeFork root;

    if ( node0->mTree && ( node0->mTree == node1->mTree )) {
    
        BlockTreeSegment seg0;
        BlockTreeSegment seg1;
    
        seg0.mTop = node0;
        seg1.mTop = node1;
    
        size_t height0 = node0->mHeader->getHeight ();
        size_t height1 = node1->mHeader->getHeight ();

        size_t height = height0 < height1 ? height0 : height1;
        
        while ( node0->mParent && ( height < node0->mHeader->getHeight ())) {
            node0 = node0->mParent.get ();
        }
        
        while ( node1->mParent && ( height < node1->mHeader->getHeight ())) {
            node1 = node1->mParent.get ();
        }

        seg0.mHead = node0;
        seg1.mHead = node1;

        seg0.mTail = node0;
        seg1.mTail = node1;

        while ( node0 != node1 ) {
        
            seg0.mHead = node0;
            seg1.mHead = node1;
            
            node0 = node0->mParent.get ();
            node1 = node1->mParent.get ();
        }
        
        assert ( node0 && node1 );
        
        root.mSeg0 = seg0;
        root.mSeg1 = seg1;
        root.mRoot = node0;
        
        assert ( root.mSeg0.getSegLength () == root.mSeg1.getSegLength ());
    }
    return root;
}

//----------------------------------------------------------------//
const BlockTreeNode* BlockTreeNode::findRoot ( const BlockTreeNode* node0, const BlockTreeNode* node1 ) {

    BlockTreeFork fork = BlockTreeNode::findFork ( node0, node1 );
    return fork.mRoot;
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
const BlockTreeNode* BlockTreeNode::getParent () const {

    return this->mParent.get ();
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
    
    string charm = ( **this ).getCharmTag ();
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
const BlockTreeNode* BlockTreeNode::trim ( Status status ) const {

    const BlockTreeNode* cursor = this;

    while ( cursor && ( cursor->mStatus & status )) {
        cursor = cursor->mParent.get ();
    }
    return cursor;
}

//----------------------------------------------------------------//
const BlockTreeNode* BlockTreeNode::trimInvalid () const {

    return this->trim ( STATUS_INVALID );
}

//----------------------------------------------------------------//
const BlockTreeNode* BlockTreeNode::trimMissing () const {

    return this->trim ( STATUS_MISSING );
}

//----------------------------------------------------------------//
const BlockTreeNode* BlockTreeNode::trimMissingOrInvalid () const {

    return this->trim (( Status )( STATUS_MISSING | STATUS_INVALID ));
}

//----------------------------------------------------------------//
string BlockTreeNode::writeBranch () const {

    string str;
    this->logBranchRecurse ( str );
    return str;
}

} // namespace Volition
