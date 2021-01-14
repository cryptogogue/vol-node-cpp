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
    InMemoryBlockTreeNode*          findNodeForHash             ( string hash );
    const InMemoryBlockTreeNode*    findNodeForHash             ( string hash ) const;
    void                            logTreeRecurse              ( string prefix, size_t maxDepth, const InMemoryBlockTreeNode* node, size_t depth ) const;

    //----------------------------------------------------------------//
    BlockTreeCursor             AbstractBlockTree_affirm                    ( BlockTreeTag& tag, shared_ptr < const BlockHeader > header, shared_ptr < const Block > block, bool isProvisional ) override;
    BlockTreeCursor             AbstractBlockTree_findCursorForHash         ( string hash ) const override;
    BlockTreeCursor             AbstractBlockTree_findCursorForTag          ( const BlockTreeTag& tag ) const override;
    shared_ptr < const Block >  AbstractBlockTree_getBlock                  ( const BlockTreeCursor& cursor ) const override;
    void                        AbstractBlockTree_mark                      ( const BlockTreeCursor& cursor, kBlockTreeEntryStatus status ) override;
    BlockTreeCursor             AbstractBlockTree_tag                       ( BlockTreeTag& tag, const BlockTreeCursor& cursor ) override;
    BlockTreeCursor             AbstractBlockTree_tag                       ( BlockTreeTag& tag, const BlockTreeTag& otherTag ) override;
    void                        AbstractBlockTree_update                    ( shared_ptr < const Block > block ) override;

public:

    //----------------------------------------------------------------//
                                InMemoryBlockTree               ();
                                ~InMemoryBlockTree              ();
    void                        logTree                         ( string prefix = "", size_t maxDepth = 0 ) const;
};

} // namespace Volition
#endif
