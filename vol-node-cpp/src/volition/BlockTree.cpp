// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Block.h>
#include <volition/BlockTree.h>
#include <volition/BlockTreeNode.h>

namespace Volition {

//================================================================//
// BlockTree
//================================================================//

//----------------------------------------------------------------//
BlockTreeNode::ConstPtr BlockTree::affirm ( shared_ptr < const BlockHeader > header, shared_ptr < const Block > block, bool isProvisional ) {

    if ( !header ) return NULL;

    string hash = header->getDigest ().toHex ();
    if ( block ) {
        assert ( hash == block->getDigest ().toHex ());
    }

    BlockTreeNode::Ptr node = this->findNodeForHash ( hash );

    if ( !node ) {

        string prevHash = header->getPrevDigest ();
        BlockTreeNode::Ptr prevNode = this->findNodeForHash ( prevHash );

        if ( !prevNode && this->mRoot ) return NULL;

        node = make_shared < BlockTreeNode >();

        node->mTree         = this;
        node->mHeader       = header;
        node->mStatus       = BlockTreeNode::STATUS_NEW;
        node->mMeta         = isProvisional ? BlockTreeNode::META_PROVISIONAL : BlockTreeNode::META_NONE;

        if ( prevNode ) {
            node->mParent = prevNode;
            prevNode->mChildren.insert ( node.get ());
            
            if (( node->mParent->mStatus == BlockTreeNode::STATUS_MISSING ) || ( node->mParent->mStatus == BlockTreeNode::STATUS_INVALID )) {
                node->mStatus = node->mParent->mStatus;
            }
        }
        else {
            this->mRoot = node;
            node->mMeta = BlockTreeNode::META_ROOT;
        }
        this->mNodes [ hash ] = node.get ();
    }
    
    this->update ( block );
    return node;
}

//----------------------------------------------------------------//
BlockTreeNode::ConstPtr BlockTree::affirmBlock ( shared_ptr < const Block > block ) {

    return this->affirm ( block, block );
}

//----------------------------------------------------------------//
BlockTreeNode::ConstPtr BlockTree::affirmHeader ( shared_ptr < const BlockHeader > header ) {

    return this->affirm ( header, NULL );
}

//----------------------------------------------------------------//
BlockTreeNode::ConstPtr BlockTree::affirmProvisional ( shared_ptr < const BlockHeader > header ) {

    return this->affirm ( header, NULL, true );
}

//----------------------------------------------------------------//
BlockTree::BlockTree () {
}

//----------------------------------------------------------------//
BlockTree::~BlockTree () {

    map < string, BlockTreeNode* >::iterator nodeIt = this->mNodes.begin ();
    for ( ; nodeIt != this->mNodes.end (); ++nodeIt ) {
        nodeIt->second->mTree = NULL;
    }
}

//----------------------------------------------------------------//
BlockTree::CanAppend BlockTree::checkAppend ( const BlockHeader& header ) const {

    if ( header.getHeight () < ( **this->mRoot ).getHeight ()) return REFUSED;

    string hash = header.getDigest ().toHex ();

    BlockTreeNode::ConstPtr node = this->findNodeForHash ( hash );
    if ( node ) return ALREADY_EXISTS;

    if ( !node ) {

        string prevHash = header.getPrevDigest ();
        BlockTreeNode::ConstPtr prevNode = this->findNodeForHash ( prevHash );

        if ( !prevNode ) return MISSING_PARENT;
        if ( prevNode->mMeta == BlockTreeNode::META_REFUSED ) return REFUSED;
        if ( header.getTime () < ( **prevNode ).getNextTime ()) return TOO_SOON;
    }
    return APPEND_OK;
}

//----------------------------------------------------------------//
BlockTreeNode::Ptr BlockTree::findNodeForHash ( string hash ) {

    map < string, BlockTreeNode* >::iterator nodeIt = this->mNodes.find ( hash );
    if ( nodeIt != this->mNodes.end ()) return nodeIt->second->shared_from_this ();
    return NULL;
}

//----------------------------------------------------------------//
BlockTreeNode::ConstPtr BlockTree::findNodeForHash ( string hash ) const {

    map < string, BlockTreeNode* >::const_iterator nodeIt = this->mNodes.find ( hash );
    if ( nodeIt != this->mNodes.cend ()) return nodeIt->second->shared_from_this ();
    return NULL;
}

//----------------------------------------------------------------//
void BlockTree::logTree ( string prefix, size_t maxDepth ) const {

    this->logTreeRecurse ( prefix, maxDepth, this->mRoot.get (), 0 );
}

//----------------------------------------------------------------//
void BlockTree::logTreeRecurse ( string prefix, size_t maxDepth, const BlockTreeNode* node, size_t depth ) const {

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
    LGN_LOG ( VOL_FILTER_ROOT, INFO, "%s%s", prefix.c_str (), str.c_str ());
    
    if ( node ) {
        ++depth;
        set < BlockTreeNode* >::const_iterator childIt = node->mChildren.begin ();
        for ( ; childIt != node->mChildren.end (); ++ childIt ) {
            this->logTreeRecurse ( prefix, maxDepth, *childIt, depth );
        }
    }
}

//----------------------------------------------------------------//
void BlockTree::mark ( BlockTreeNode::ConstPtr node, BlockTreeNode::Status status ) {

    if ( !node ) return;
    
    BlockTreeNode::Ptr cursor = this->findNodeForHash (( **node ).getDigest ());
    if ( cursor ) {
        cursor->mark ( status );
    }
}

//----------------------------------------------------------------//
void BlockTree::setRoot ( BlockTreeNode::ConstPtr node ) {

    if ( this->mRoot == node ) return;
    if ( !node ) return;
    if ( node->mMeta == BlockTreeNode::META_ROOT ) return;
    assert ( node->mStatus & BlockTreeNode::STATUS_COMPLETE );

    BlockTreeNode::Ptr cursor = this->findNodeForHash (( **node ).getDigest ());
    assert ( cursor );
    
    cursor->mMeta = BlockTreeNode::META_ROOT;
    
    shared_ptr < BlockTreeNode > prevRoot = this->mRoot;
    this->mRoot = cursor;
    cursor->clearParent ();
    
    if ( prevRoot ) {
        prevRoot->markRefused ();
    }
}

//----------------------------------------------------------------//
BlockTreeNode::ConstPtr BlockTree::update ( shared_ptr < const Block > block ) {

    if ( !block ) return NULL;
    string hash = block->getDigest ();

    BlockTreeNode::Ptr node = this->findNodeForHash ( hash );
    if ( !node ) return NULL;
    
    assert ( node->mHeader );
    assert ( node->mHeader->getDigest ().toHex () == hash );
    
    node->mBlock = block;
    node->markComplete ();
    
    return node;
}

} // namespace Volition
