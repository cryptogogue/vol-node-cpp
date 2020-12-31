// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_BLOCKTREE_H
#define VOLITION_BLOCKTREE_H

#include <volition/common.h>
#include <volition/Accessors.h>
#include <volition/Block.h>
#include <volition/BlockTreeNode.h>
#include <volition/BlockTreeNodeTag.h>

namespace Volition {

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

    friend class BlockTreeNode;
    friend class BlockTreeNodeTag;

    BlockTreeNode*                                  mRoot;
    map < string, BlockTreeNode* >                  mNodes;
    map < string, shared_ptr < BlockTreeNode >>     mTags;

    //----------------------------------------------------------------//
    const BlockTreeNode*                affirm                  ( BlockTreeNodeTag& tag, shared_ptr < const BlockHeader > header, shared_ptr < const Block > block, bool isProvisional = false );
    BlockTreeNode::Ptr                  findNodeForHash         ( string hash );
    const BlockTreeNode*                findNodeForTagName      ( string tagName ) const;
    void                                logTreeRecurse          ( string prefix, size_t maxDepth, const BlockTreeNode* node, size_t depth ) const;
    void                                tag                     ( string tagName, string otherTagName );

public:

    GET ( BlockTreeNode::ConstPtr, Root, mRoot )

    //----------------------------------------------------------------//
    const BlockTreeNode*                affirmBlock             ( BlockTreeNodeTag& tag, shared_ptr < const Block > block );
    const BlockTreeNode*                affirmHeader            ( BlockTreeNodeTag& tag, shared_ptr < const BlockHeader > header );
    const BlockTreeNode*                affirmProvisional       ( BlockTreeNodeTag& tag, shared_ptr < const BlockHeader > header );
                                        BlockTree               ();
                                        ~BlockTree              ();
    CanAppend                           checkAppend             ( const BlockHeader& header ) const;
    BlockTreeNode::ConstPtr             findNodeForHash         ( string hash ) const;
    void                                logTree                 ( string prefix = "", size_t maxDepth = 0 ) const;
    void                                mark                    ( BlockTreeNode::ConstPtr node, BlockTreeNode::Status status );
    const BlockTreeNode*                tag                     ( BlockTreeNodeTag& tag, BlockTreeNode::ConstPtr node );
    BlockTreeNode::ConstPtr             update                  ( shared_ptr < const Block > block );
};

} // namespace Volition
#endif
