// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_ABSTRACTBLOCKTREE_H
#define VOLITION_ABSTRACTBLOCKTREE_H

#include <volition/common.h>
#include <volition/Block.h>
#include <volition/AbstractBlockTreeNode.h>

namespace Volition {

//================================================================//
// BlockTree
//================================================================//
class BlockTree :
    public enable_shared_from_this < BlockTree > {
public:

    enum CanAppend {
        APPEND_OK,
        ALREADY_EXISTS,
        MISSING_PARENT,
        REFUSED,
        TOO_SOON,
    };

private:

    friend class BlockTreeNode;

    //----------------------------------------------------------------//
    BlockTreeNodeConstPtr               affirm                  ( string tagName, shared_ptr < const BlockHeader > header, shared_ptr < const Block > block, bool isProvisional = false );
    BlockTreeNodePtr                    findNodeForHash         ( string hash );
    void                                logTreeRecurse          ( string prefix, size_t maxDepth, const BlockTreeNode* node, size_t depth ) const;

public:

    GET ( BlockTreeNode::ConstPtr, Root, mRoot )

    //----------------------------------------------------------------//
    BlockTreeNodeConstPtr               affirmBlock             ( string tagName, shared_ptr < const Block > block );
    BlockTreeNodeConstPtr               affirmHeader            ( string tagName, shared_ptr < const BlockHeader > header );
    BlockTreeNodeConstPtr               affirmProvisional       ( string tagName, shared_ptr < const BlockHeader > header );
                                        BlockTree               ();
                                        ~BlockTree              ();
    CanAppend                           checkAppend             ( const BlockHeader& header ) const;
    BlockTreeNodeConstPtr               findNodeForHash         ( string hash ) const;
    void                                mark                    ( BlockTreeNodeConstPtr node, BlockTreeNode::Status status );
    BlockTreeNodeConstPtr               update                  ( shared_ptr < const Block > block );
};

} // namespace Volition
#endif
