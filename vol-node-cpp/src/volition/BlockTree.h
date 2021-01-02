// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_BLOCKTREE_H
#define VOLITION_BLOCKTREE_H

#include <volition/common.h>
#include <volition/AbstractBlockTree.h>
#include <volition/Accessors.h>
#include <volition/Block.h>
#include <volition/BlockTreeCursor.h>
#include <volition/BlockTreeNode.h>
#include <volition/BlockTreeTag.h>

namespace Volition {

//================================================================//
// BlockTreeSegment
//================================================================//
class BlockTreeSegment {
public:

    const BlockTreeNode*    mHead;
    const BlockTreeNode*    mTail;
    const BlockTreeNode*    mTop;
    
    //----------------------------------------------------------------//
    size_t          getFullLength           () const;
    size_t          getRewriteDefeatCount   () const;
    size_t          getSegLength            () const;
};

//================================================================//
// BlockTreeRoot
//================================================================//
class BlockTreeFork {
public:

    const BlockTreeNode*    mRoot;
    BlockTreeSegment        mSeg0;
    BlockTreeSegment        mSeg1;
    
    //----------------------------------------------------------------//
    size_t          getSegLength            () const;
};

//================================================================//
// BlockTree
//================================================================//
class BlockTree :
    public AbstractBlockTree {
private:

    friend class BlockTreeCursor;
    friend class BlockTreeNode;
    friend class BlockTreeTag;

    BlockTreeNode*                                  mRoot;
    map < string, BlockTreeNode* >                  mNodes;
    map < string, shared_ptr < BlockTreeNode >>     mTags;

    //----------------------------------------------------------------//
    BlockTreeFork               findFork                ( const BlockTreeCursor& cursor0, const BlockTreeCursor& cursor1 ) const;
    BlockTreeNode*              findNodeForHash         ( string hash );
    const BlockTreeNode*        findNodeForHash         ( string hash ) const;
    void                        logTreeRecurse          ( string prefix, size_t maxDepth, const BlockTreeNode* node, size_t depth ) const;

    //----------------------------------------------------------------//
    BlockTreeCursor             AbstractBlockTree_affirm                    ( BlockTreeTag& tag, shared_ptr < const BlockHeader > header, shared_ptr < const Block > block, bool isProvisional = false ) override;
    CanAppend                   AbstractBlockTree_checkAppend               ( const BlockHeader& header ) const override;
    int                         AbstractBlockTree_compare                   ( const BlockTreeCursor& cursor0, const BlockTreeCursor& cursor1, BlockTreeCursor::RewriteMode rewriteMode ) const override;
    BlockTreeCursor             AbstractBlockTree_findCursorForHash         ( string hash ) const override;
    BlockTreeCursor             AbstractBlockTree_findCursorForTag          ( const BlockTreeTag& tag ) const override;
    BlockTreeCursor             AbstractBlockTree_findRoot                  ( const BlockTreeCursor& cursor0, const BlockTreeCursor& cursor1 ) const override;
    BlockTreeCursor             AbstractBlockTree_getParent                 ( const BlockTreeCursor& cursor ) const override;
    void                        AbstractBlockTree_mark                      ( const BlockTreeCursor& cursor, BlockTreeCursor::Status status ) override;
    BlockTreeCursor             AbstractBlockTree_tag                       ( BlockTreeTag& tag, const BlockTreeCursor& cursor ) override;
    BlockTreeCursor             AbstractBlockTree_tag                       ( BlockTreeTag& tag, const BlockTreeTag& otherTag ) override;
    BlockTreeCursor             AbstractBlockTree_update                    ( shared_ptr < const Block > block ) override;

public:

    GET ( BlockTreeNode::ConstPtr, Root, mRoot )

    //----------------------------------------------------------------//
                                BlockTree               ();
                                ~BlockTree              ();
    void                        logTree                 ( string prefix = "", size_t maxDepth = 0 ) const;
};

} // namespace Volition
#endif
