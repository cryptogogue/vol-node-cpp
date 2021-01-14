// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Block.h>
#include <volition/InMemoryBlockTree.h>
#include <volition/InMemoryBlockTreeNode.h>

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
// InMemoryBlockTreeNode
//================================================================//

//----------------------------------------------------------------//
void InMemoryBlockTreeNode::clearParent () {

    if ( this->mParent ) {
        this->mParent->mChildren.erase ( this );
    }
    this->mParent = NULL;
}

//----------------------------------------------------------------//
InMemoryBlockTreeNode::InMemoryBlockTreeNode ( InMemoryBlockTree& tree, shared_ptr < const BlockHeader > header, bool isProvisional ) :
    mInMemoryBlockTree ( &tree ) {
    
    assert ( header );
    this->mTree     = &tree;
    this->mHeader   = header;
    this->mStatus   = kBlockTreeEntryStatus::STATUS_NEW;
    this->mMeta     = isProvisional ? kBlockTreeEntryMeta::META_PROVISIONAL : kBlockTreeEntryMeta::META_NONE;
}

//----------------------------------------------------------------//
InMemoryBlockTreeNode::~InMemoryBlockTreeNode () {

    InMemoryBlockTree* tree = this->mInMemoryBlockTree;

    assert ( tree );
    assert ( this->mHeader );
    tree->mNodes.erase ( this->mHeader->getDigest ());
    this->clearParent ();
}

//----------------------------------------------------------------//
void InMemoryBlockTreeNode::mark ( kBlockTreeEntryStatus status ) {

    if ( status == this->mStatus ) return;

    if ( status == STATUS_COMPLETE ) {
        if ( !this->mBlock ) return;
        if ( this->mParent && ( this->mParent->mStatus != STATUS_COMPLETE )) return;
    }

    assert ( AbstractBlockTree::checkStatusTransition ( this->mStatus, status ));
    
    this->mStatus = status;
    
    set < InMemoryBlockTreeNode* >::iterator childIt = this->mChildren.begin ();
    for ( ; childIt != this->mChildren.end (); ++childIt ) {
        ( *childIt )->mark ( status );
    }
}

} // namespace Volition
