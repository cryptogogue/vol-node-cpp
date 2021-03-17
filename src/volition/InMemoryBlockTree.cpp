// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Block.h>
#include <volition/InMemoryBlockTree.h>
#include <volition/InMemoryBlockTreeNode.h>

namespace Volition {

//================================================================//
// InMemoryBlockTree
//================================================================//

//----------------------------------------------------------------//
InMemoryBlockTree::InMemoryBlockTree () :
    mRoot ( NULL ) {
}

//----------------------------------------------------------------//
InMemoryBlockTree::~InMemoryBlockTree () {

    map < string, InMemoryBlockTreeNode* >::iterator nodeIt = this->mNodes.begin ();
    for ( ; nodeIt != this->mNodes.end (); ++nodeIt ) {
        nodeIt->second->mTree = NULL;
    }
}

//----------------------------------------------------------------//
InMemoryBlockTreeNode* InMemoryBlockTree::findNodeForHash ( string hash ) {

    map < string, InMemoryBlockTreeNode* >::iterator nodeIt = this->mNodes.find ( hash );
    if ( nodeIt != this->mNodes.end ()) return nodeIt->second;
    return NULL;
}

//----------------------------------------------------------------//
const InMemoryBlockTreeNode* InMemoryBlockTree::findNodeForHash ( string hash ) const {

    map < string, InMemoryBlockTreeNode* >::const_iterator nodeIt = this->mNodes.find ( hash );
    if ( nodeIt != this->mNodes.cend ()) return nodeIt->second;
    return NULL;
}

//----------------------------------------------------------------//
void InMemoryBlockTree::logTree ( string prefix, size_t maxDepth ) const {

    this->logTreeRecurse ( prefix, maxDepth, this->mRoot, 0 );
}

//----------------------------------------------------------------//
void InMemoryBlockTree::logTreeRecurse ( string prefix, size_t maxDepth, const InMemoryBlockTreeNode* node, size_t depth ) const {

    if ( !node ) return;
    if (( maxDepth > 0 ) && ( depth >= maxDepth )) return;

    string str;
    Format::write_indent ( str, ".   ", depth );
    
    int i = 0;
    do {
        const BlockHeader& header = *node->mHeader;
        Format::write ( str, "%s[%s]", ( i > 0 ) ? "," : "", ( header.getHeight () > 0 ) ? header.getMinerID ().c_str () : "-" );
        node = ( node->mChildren.size () > 0 ) ? *node->mChildren.begin () : NULL;
        ++i;
    }
    while ( node && ( node->mChildren.size () <= 1 ));
    LGN_LOG ( VOL_FILTER_CONSENSUS, INFO, "%s%s", prefix.c_str (), str.c_str ());
    
    if ( node ) {
        ++depth;
        set < InMemoryBlockTreeNode* >::const_iterator childIt = node->mChildren.begin ();
        for ( ; childIt != node->mChildren.end (); ++ childIt ) {
            this->logTreeRecurse ( prefix, maxDepth, *childIt, depth );
        }
    }
}

//================================================================//
// virtual
//================================================================//

//----------------------------------------------------------------//
BlockTreeCursor InMemoryBlockTree::AbstractBlockTree_affirm ( BlockTreeTag& tag, shared_ptr < const BlockHeader > header, shared_ptr < const Block > block, bool isProvisional ) {

    string tagName = tag.getName ();
    string hash = header->getDigest ().toHex ();
    InMemoryBlockTreeNode::Ptr node = this->findNodeForHash ( hash );

    if ( node ) {
        this->mTags [ tagName ] = node->shared_from_this ();
    }
    else {

        string prevHash = header->getPrevDigest ();
        InMemoryBlockTreeNode* prevNode = this->findNodeForHash ( prevHash );

        if ( !prevNode && this->mRoot ) return BlockTreeCursor ();

        shared_ptr < InMemoryBlockTreeNode > shared = make_shared < InMemoryBlockTreeNode >( *this, header, isProvisional );
        node = shared.get ();

        if ( prevNode ) {
            node->mParent = prevNode->shared_from_this ();
            prevNode->mChildren.insert ( node );
            node->mBranchStatus = prevNode->mBranchStatus == BRANCH_STATUS_COMPLETE ? BRANCH_STATUS_NEW : prevNode->mBranchStatus;
        }
        else {
            this->mRoot = node;
        }
        this->mNodes [ hash ] = node;
        this->mTags [ tagName ] = shared;
    }
    
    this->update ( block );
    return *node;
}

//----------------------------------------------------------------//
BlockTreeCursor InMemoryBlockTree::AbstractBlockTree_findCursorForHash ( string hash ) const {

    map < string, InMemoryBlockTreeNode* >::const_iterator nodeIt = this->mNodes.find ( hash );
    if ( nodeIt != this->mNodes.cend ()) return *nodeIt->second;
    return BlockTreeCursor ();
}

//----------------------------------------------------------------//
BlockTreeCursor InMemoryBlockTree::AbstractBlockTree_findCursorForTagName ( string tagName ) const {

    map < string, shared_ptr < InMemoryBlockTreeNode >>::const_iterator nodeIt = this->mTags.find ( tagName );
    if ( nodeIt != this->mTags.cend ()) return *nodeIt->second;

    return BlockTreeCursor ();
}

//----------------------------------------------------------------//
shared_ptr < const Block > InMemoryBlockTree::AbstractBlockTree_getBlock ( const BlockTreeCursor& cursor ) const {

    assert ( cursor.getTree () == this );
    if ( !cursor.hasBlock ()) return NULL;

    const InMemoryBlockTreeNode* node = this->findNodeForHash ( cursor.getHash ());
    assert ( node && node->mBlock );
    
    return node->mBlock;
}

//----------------------------------------------------------------//
void InMemoryBlockTree::AbstractBlockTree_setBranchStatus ( const BlockTreeCursor& cursor, kBlockTreeBranchStatus status ) {

    InMemoryBlockTreeNode* node = this->findNodeForHash ( cursor.getHash ());

    if ( node ) {
        node->setBranchStatus ( status );
    }
}

//----------------------------------------------------------------//
void InMemoryBlockTree::AbstractBlockTree_setSearchStatus ( const BlockTreeCursor& cursor, kBlockTreeSearchStatus status ) {

    InMemoryBlockTreeNode* node = this->findNodeForHash ( cursor.getHash ());
    node->mSearchStatus = status;
}

//----------------------------------------------------------------//
BlockTreeCursor InMemoryBlockTree::AbstractBlockTree_tag ( BlockTreeTag& tag, const BlockTreeCursor& cursor ) {
        
    InMemoryBlockTreeNode::Ptr node = this->findNodeForHash ( cursor.getHash ());
    assert ( node );
    this->mTags [ tag.getName ()] = node->shared_from_this ();
    
    return *node;
}

//----------------------------------------------------------------//
BlockTreeCursor InMemoryBlockTree::AbstractBlockTree_tag ( BlockTreeTag& tag, const BlockTreeTag& otherTag ) {
        
    map < string, shared_ptr < InMemoryBlockTreeNode >>::const_iterator nodeIt = this->mTags.find ( otherTag.getName ());
    assert ( nodeIt != this->mTags.cend ());
    this->mTags [ tag.getName ()] = nodeIt->second;
    
    return *nodeIt->second;
}

//----------------------------------------------------------------//
void InMemoryBlockTree::AbstractBlockTree_update ( shared_ptr < const Block > block ) {

    if ( !block ) return;
    string hash = block->getDigest ();

    InMemoryBlockTreeNode::Ptr node = this->findNodeForHash ( hash );
    if ( !node ) return;
    
    assert ( node->mHeader );
    assert ( node->mHeader->getDigest ().toHex () == hash );
    
    node->mBlock = block;
    node->mSearchStatus = kBlockTreeSearchStatus::SEARCH_STATUS_HAS_BLOCK;
    node->setBranchStatus ( kBlockTreeBranchStatus::BRANCH_STATUS_COMPLETE );
}

} // namespace Volition
