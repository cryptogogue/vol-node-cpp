// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_BLOCKTREE_H
#define VOLITION_BLOCKTREE_H

#include <volition/common.h>
#include <volition/Block.h>

namespace Volition {

class BlockTree;

//================================================================//
// BlockTreeNode
//================================================================//
class BlockTreeNode :
    public enable_shared_from_this < BlockTreeNode > {
private:

    friend class BlockTree;

    BlockTree*                          mTree;

    shared_ptr < const BlockHeader >    mHeader;
    shared_ptr < const Block >          mBlock;
    shared_ptr < BlockTreeNode >        mParent;
    set < BlockTreeNode* >              mChildren;

public:

    //----------------------------------------------------------------//
                    BlockTreeNode           ();
                    ~BlockTreeNode          ();
};

//================================================================//
// BlockTree
//================================================================//
class BlockTree :
    public enable_shared_from_this < BlockTreeNode > {
private:

    friend class BlockTreeNode;

    shared_ptr < BlockTreeNode >        mRoot;
    map < string, BlockTreeNode* >      mNodes;

public:

    //----------------------------------------------------------------//
    shared_ptr < BlockTreeNode >        affirmNode              ( shared_ptr < const Block > block );
    shared_ptr < BlockTreeNode >        affirmNode              ( shared_ptr < const BlockHeader > header, shared_ptr < const Block > block );
                                        BlockTree               ();
                                        ~BlockTree              ();
    shared_ptr < BlockTreeNode >        findNodeForHash         ( string hash );
};

} // namespace Volition
#endif
