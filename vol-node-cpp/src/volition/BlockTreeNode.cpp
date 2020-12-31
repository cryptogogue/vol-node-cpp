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
// BlockTreeNode
//================================================================//

//----------------------------------------------------------------//
BlockTreeNode::BlockTreeNode () {
}

//----------------------------------------------------------------//
BlockTreeNode::~BlockTreeNode () {

    if ( this->mTree && this->mHeader ) {
        this->mTree->mNodes.erase ( this->mHeader->getDigest ());
    }
    this->clearParent ();
}

//----------------------------------------------------------------//
void BlockTreeNode::clearParent () {

    if ( this->mParent ) {
        this->mParent->mChildren.erase ( this );
    }
    this->mParent = NULL;
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

} // namespace Volition
