// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_BLOCKTREE_H
#define VOLITION_BLOCKTREE_H

#include <volition/common.h>
#include <volition/Accessors.h>
#include <volition/Block.h>
#include <volition/BlockTreeNode.h>

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

    shared_ptr < BlockTreeNode >        mRoot;
    map < string, BlockTreeNode* >      mNodes;

    //----------------------------------------------------------------//
    BlockTreeNode::ConstPtr             affirm                  ( shared_ptr < const BlockHeader > header, shared_ptr < const Block > block, bool isProvisional = false );
    void                                erase                   ( string hash );
    BlockTreeNode::Ptr                  findNodeForHash         ( string hash );
    void                                logTreeRecurse          ( string prefix, size_t maxDepth, const BlockTreeNode* node, size_t depth ) const;

public:

    GET ( BlockTreeNode::ConstPtr, Root, mRoot )

    //----------------------------------------------------------------//
    BlockTreeNode::ConstPtr             affirmBlock             ( shared_ptr < const Block > block );
    BlockTreeNode::ConstPtr             affirmHeader            ( shared_ptr < const BlockHeader > header );
    BlockTreeNode::ConstPtr             affirmProvisional       ( shared_ptr < const BlockHeader > header );
                                        BlockTree               ();
                                        ~BlockTree              ();
    CanAppend                           checkAppend             ( const BlockHeader& header ) const;
    BlockTreeNode::ConstPtr             findNodeForHash         ( string hash ) const;
    void                                logTree                 ( string prefix = "", size_t maxDepth = 0 ) const;
    void                                mark                    ( BlockTreeNode::ConstPtr node, BlockTreeNode::Status status );
    void                                setRoot                 ( BlockTreeNode::ConstPtr node );
    BlockTreeNode::ConstPtr             update                  ( shared_ptr < const Block > block );
};

} // namespace Volition
#endif
