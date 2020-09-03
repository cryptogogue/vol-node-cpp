// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Block.h>
#include <volition/BlockTree.h>

namespace Volition {

//================================================================//
// BlockTreeNode
//================================================================//

//----------------------------------------------------------------//
BlockTreeNode::BlockTreeNode () :
    mTree ( NULL ) {
}

//----------------------------------------------------------------//
BlockTreeNode::~BlockTreeNode () {

    if ( this->mTree && this->mHeader ) {
        this->mTree->mNodes.erase ( this->mHeader->getHash ());
    }
    
    if ( this->mParent ) {
        this->mParent->mChildren.erase ( this );
    }
}

//================================================================//
// BlockTree
//================================================================//

//----------------------------------------------------------------//
shared_ptr < BlockTreeNode > BlockTree::affirmNode ( shared_ptr < const Block > block ) {

    return this->affirmNode ( block, block );
}

//----------------------------------------------------------------//
shared_ptr < BlockTreeNode > BlockTree::affirmNode ( shared_ptr < const BlockHeader > header, shared_ptr < const Block > block ) {

    if ( !( header && block )) return NULL;

    string hash = header->getHash ();
    if ( block ) {
        assert ( hash == block->getHash ());
    }

    shared_ptr < BlockTreeNode > node = this->findNodeForHash ( hash );;

    if ( node ) {
        if ( block ) {
            node->mBlock = block;
        }
        return node;
    }

    string prevHash = header->getPrevHash ();
    shared_ptr < BlockTreeNode > prevNode = this->findNodeForHash ( prevHash );
    
    if ( !prevNode && ( this->mRoot )) return NULL; // already have a root

    node = make_shared < BlockTreeNode >();

    node->mTree     = this;
    node->mHeader   = header;
    node->mBlock    = block;

    if ( prevNode ) {
        node->mParent = prevNode;
        prevNode->mChildren.insert ( node.get ());
    }
    else {
        this->mRoot = node;
    }

    this->mNodes [ hash ] = node.get ();
    return node;
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
shared_ptr < BlockTreeNode > BlockTree::findNodeForHash ( string hash ) {

    map < string, BlockTreeNode* >::iterator nodeIt = this->mNodes.find ( hash );
    if ( nodeIt != this->mNodes.end ()) return nodeIt->second->shared_from_this ();
    return NULL;
}

} // namespace Volition
