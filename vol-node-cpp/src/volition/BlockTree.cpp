// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Block.h>
#include <volition/BlockTree.h>

namespace Volition {

//================================================================//
// BlockTree
//================================================================//

//----------------------------------------------------------------//
const BlockTreeNode* BlockTree::affirm ( BlockTreeNodeTag& tag, shared_ptr < const BlockHeader > header, shared_ptr < const Block > block, bool isProvisional ) {

    string tagName = tag.mTagName;
    assert ( tagName.size ());
    assert (( tag.mBlockTree == NULL ) || ( tag.mBlockTree == this ));
    
    tag.mBlockTree = this;

    if ( !header ) return NULL;

    string hash = header->getDigest ().toHex ();
    if ( block ) {
        assert ( hash == block->getDigest ().toHex ());
    }
    
    BlockTreeNode::Ptr node = this->findNodeForHash ( hash );

    if ( node ) {
        this->mTags [ tagName ] = node->shared_from_this ();
    }
    else {

        string prevHash = header->getPrevDigest ();
        BlockTreeNode* prevNode = this->findNodeForHash ( prevHash );

        if ( !prevNode && this->mRoot ) return NULL;

        shared_ptr < BlockTreeNode > shared = make_shared < BlockTreeNode >();
        node = shared.get ();

        node->mTree         = this;
        node->mHeader       = header;
        node->mStatus       = BlockTreeNode::STATUS_NEW;
        node->mMeta         = isProvisional ? BlockTreeNode::META_PROVISIONAL : BlockTreeNode::META_NONE;

        if ( prevNode ) {
        
            node->mParent = prevNode->shared_from_this ();
            prevNode->mChildren.insert ( node );
            
            if (( node->mParent->mStatus == BlockTreeNode::STATUS_MISSING ) || ( node->mParent->mStatus == BlockTreeNode::STATUS_INVALID )) {
                node->mStatus = node->mParent->mStatus;
            }
        }
        else {
            this->mRoot = node;
        }
        this->mNodes [ hash ] = node;
        this->mTags [ tagName ] = shared;
    }
    
    this->update ( block );
    return node;
}

//----------------------------------------------------------------//
const BlockTreeNode* BlockTree::affirmBlock ( BlockTreeNodeTag& tag, shared_ptr < const Block > block ) {

    return this->affirm ( tag, block, block );
}

//----------------------------------------------------------------//
const BlockTreeNode* BlockTree::affirmHeader ( BlockTreeNodeTag& tag, shared_ptr < const BlockHeader > header ) {

    return this->affirm ( tag, header, NULL );
}

//----------------------------------------------------------------//
const BlockTreeNode* BlockTree::affirmProvisional ( BlockTreeNodeTag& tag, shared_ptr < const BlockHeader > header ) {

    return this->affirm ( tag, header, NULL, true );
}

//----------------------------------------------------------------//
BlockTree::BlockTree () :
    mRoot ( NULL ) {
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
    if ( nodeIt != this->mNodes.end ()) return nodeIt->second;
    return NULL;
}

//----------------------------------------------------------------//
BlockTreeNode::ConstPtr BlockTree::findNodeForHash ( string hash ) const {

    map < string, BlockTreeNode* >::const_iterator nodeIt = this->mNodes.find ( hash );
    if ( nodeIt != this->mNodes.cend ()) return nodeIt->second;
    return NULL;
}

//----------------------------------------------------------------//
const BlockTreeNode* BlockTree::findNodeForTagName ( string tagName ) const {

    map < string, shared_ptr < BlockTreeNode >>::const_iterator nodeIt = this->mTags.find ( tagName );
    if ( nodeIt != this->mTags.cend ()) return nodeIt->second.get ();
    return NULL;
}

//----------------------------------------------------------------//
void BlockTree::logTree ( string prefix, size_t maxDepth ) const {

    this->logTreeRecurse ( prefix, maxDepth, this->mRoot, 0 );
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
void BlockTree::tag ( string tagName, string otherTagName ) {

    assert ( tagName.size ());
    assert ( otherTagName.size ());

    map < string, shared_ptr < BlockTreeNode >>::const_iterator nodeIt = this->mTags.find ( otherTagName );
    if ( nodeIt != this->mTags.cend ()) {
        this->mTags [ tagName ] = nodeIt->second;
    }
}

//----------------------------------------------------------------//
const BlockTreeNode* BlockTree::tag ( BlockTreeNodeTag& tag, BlockTreeNode::ConstPtr node ) {

    string tagName = tag.mTagName;
    assert ( tagName.size ());
    assert (( tag.mBlockTree == NULL ) || ( tag.mBlockTree == this ));
    
    tag.mBlockTree = this;
    
    BlockTreeNode::Ptr cursor = this->findNodeForHash (( **node ).getDigest ());
    assert ( cursor );
    this->mTags [ tagName ] = cursor->shared_from_this ();
    
    return node;
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
