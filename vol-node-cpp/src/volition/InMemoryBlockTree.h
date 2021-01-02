// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_INMEMORYBLOCKTREE_H
#define VOLITION_INMEMORYBLOCKTREE_H

#include <volition/common.h>
#include <volition/AbstractBlockTree.h>
#include <volition/Accessors.h>
#include <volition/Block.h>
#include <volition/BlockTreeCursor.h>
#include <volition/BlockTreeTag.h>

namespace Volition {

class InMemoryBlockTreeNode;

//================================================================//
// InMemoryBlockTreeSegment
//================================================================//
class InMemoryBlockTreeSegment {
public:

    const InMemoryBlockTreeNode*    mHead;
    const InMemoryBlockTreeNode*    mTail;
    const InMemoryBlockTreeNode*    mTop;
    
    //----------------------------------------------------------------//
    size_t          getFullLength           () const;
    size_t          getRewriteDefeatCount   () const;
    size_t          getSegLength            () const;
};

//================================================================//
// InMemoryBlockTreeFork
//================================================================//
class InMemoryBlockTreeFork {
public:

    const InMemoryBlockTreeNode*        mRoot;
    InMemoryBlockTreeSegment    mSeg0;
    InMemoryBlockTreeSegment    mSeg1;
    
    //----------------------------------------------------------------//
    size_t          getSegLength            () const;
};

//================================================================//
// InMemoryBlockTree
//================================================================//
class InMemoryBlockTree :
    public AbstractBlockTree {
private:

    friend class InMemoryBlockTreeNode;

    InMemoryBlockTreeNode*                                  mRoot;
    map < string, InMemoryBlockTreeNode* >                  mNodes;
    map < string, shared_ptr < InMemoryBlockTreeNode >>     mTags;

    //----------------------------------------------------------------//
    InMemoryBlockTreeFork       findFork                        ( const BlockTreeCursor& cursor0, const BlockTreeCursor& cursor1 ) const;
    InMemoryBlockTreeNode*              findNodeForHash         ( string hash );
    const InMemoryBlockTreeNode*        findNodeForHash         ( string hash ) const;
    void                        logTreeRecurse                  ( string prefix, size_t maxDepth, const InMemoryBlockTreeNode* node, size_t depth ) const;

    //----------------------------------------------------------------//
    BlockTreeCursor             AbstractBlockTree_affirm                    ( BlockTreeTag& tag, shared_ptr < const BlockHeader > header, shared_ptr < const Block > block, bool isProvisional = false ) override;
    kBlockTreeAppendResult      AbstractBlockTree_checkAppend               ( const BlockHeader& header ) const override;
    int                         AbstractBlockTree_compare                   ( const BlockTreeCursor& cursor0, const BlockTreeCursor& cursor1, kRewriteMode rewriteMode ) const override;
    BlockTreeCursor             AbstractBlockTree_findCursorForHash         ( string hash ) const override;
    BlockTreeCursor             AbstractBlockTree_findCursorForTag          ( const BlockTreeTag& tag ) const override;
    BlockTreeCursor             AbstractBlockTree_findRoot                  ( const BlockTreeCursor& cursor0, const BlockTreeCursor& cursor1 ) const override;
    BlockTreeCursor             AbstractBlockTree_getParent                 ( const BlockTreeCursor& cursor ) const override;
    void                        AbstractBlockTree_mark                      ( const BlockTreeCursor& cursor, kBlockTreeEntryStatus status ) override;
    BlockTreeCursor             AbstractBlockTree_tag                       ( BlockTreeTag& tag, const BlockTreeCursor& cursor ) override;
    BlockTreeCursor             AbstractBlockTree_tag                       ( BlockTreeTag& tag, const BlockTreeTag& otherTag ) override;
    BlockTreeCursor             AbstractBlockTree_update                    ( shared_ptr < const Block > block ) override;

public:

    //----------------------------------------------------------------//
                                InMemoryBlockTree               ();
                                ~InMemoryBlockTree              ();
    void                        logTree                         ( string prefix = "", size_t maxDepth = 0 ) const;
};

} // namespace Volition
#endif
