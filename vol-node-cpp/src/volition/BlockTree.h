// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_BLOCKTREE_H
#define VOLITION_BLOCKTREE_H

#include <volition/common.h>
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
class BlockTree {
public:

    enum CanAppend {
        APPEND_OK,
        ALREADY_EXISTS,
        MISSING_PARENT,
        REFUSED,
        TOO_SOON,
    };

private:

    friend class BlockTreeCursor;
    friend class BlockTreeNode;
    friend class BlockTreeTag;

    BlockTreeNode*                                  mRoot;
    map < string, BlockTreeNode* >                  mNodes;
    map < string, shared_ptr < BlockTreeNode >>     mTags;

    //----------------------------------------------------------------//
    BlockTreeCursor             affirm                  ( BlockTreeTag& tag, shared_ptr < const BlockHeader > header, shared_ptr < const Block > block, bool isProvisional = false );
    int                         compare                 ( const BlockTreeCursor& cursor0, const BlockTreeCursor& cursor1, BlockTreeCursor::RewriteMode rewriteMode ) const;
    BlockTreeFork               findFork                ( const BlockTreeCursor& cursor0, const BlockTreeCursor& cursor1 ) const;
    BlockTreeCursor             findCursorForTagName    ( string tagName ) const;
    BlockTreeNode*              findNodeForHash         ( string hash );
    const BlockTreeNode*        findNodeForHash         ( string hash ) const;
    BlockTreeCursor             findRoot                ( const BlockTreeCursor& cursor0, const BlockTreeCursor& cursor1 ) const;
    BlockTreeCursor             getParent               ( const BlockTreeCursor& cursor ) const;
    void                        logTreeRecurse          ( string prefix, size_t maxDepth, const BlockTreeNode* node, size_t depth ) const;
    BlockTreeCursor             tag                     ( string tagName, string otherTagName );

public:

    GET ( BlockTreeNode::ConstPtr, Root, mRoot )

    //----------------------------------------------------------------//
    BlockTreeCursor             affirmBlock             ( BlockTreeTag& tag, shared_ptr < const Block > block );
    BlockTreeCursor             affirmHeader            ( BlockTreeTag& tag, shared_ptr < const BlockHeader > header );
    BlockTreeCursor             affirmProvisional       ( BlockTreeTag& tag, shared_ptr < const BlockHeader > header );
                                BlockTree               ();
                                ~BlockTree              ();
    CanAppend                   checkAppend             ( const BlockHeader& header ) const;
    BlockTreeCursor             findCursorForHash       ( string hash ) const;
    void                        logTree                 ( string prefix = "", size_t maxDepth = 0 ) const;
    void                        mark                    ( const BlockTreeCursor& cursor, BlockTreeNode::Status status );
    BlockTreeCursor             tag                     ( BlockTreeTag& tag, const BlockTreeCursor& cursor );
    BlockTreeCursor             update                  ( shared_ptr < const Block > block );
};

} // namespace Volition
#endif
