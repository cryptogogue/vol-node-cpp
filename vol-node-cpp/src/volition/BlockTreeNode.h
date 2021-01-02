// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_BLOCKTREENODE_H
#define VOLITION_BLOCKTREENODE_H

#include <volition/common.h>
#include <volition/Accessors.h>
#include <volition/Block.h>
#include <volition/BlockTreeCursor.h>

namespace Volition {

class BlockTree;
class BlockTreeNode;

//================================================================//
// BlockTreeNode
//================================================================//
class BlockTreeNode :
    public BlockTreeCursor,
    public enable_shared_from_this < BlockTreeNode > {
public:

    typedef BlockTreeNode*              Ptr;
    typedef const BlockTreeNode*        ConstPtr;

private:

    friend class BlockTree;
    friend class BlockTreeNode;
    friend class BlockTreeTag;

    shared_ptr < BlockTreeNode >        mParent;
    set < BlockTreeNode* >              mChildren;

    //----------------------------------------------------------------//
    void                                clearParent                 ();
    void                                mark                        ( BlockTreeNode::Status status );
    void                                markComplete                ();
    void                                markRefused                 ();

public:

    //----------------------------------------------------------------//
                                        BlockTreeNode               ();
                                        ~BlockTreeNode              ();
};

} // namespace Volition
#endif
