// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_BLOCKTREE_H
#define VOLITION_BLOCKTREE_H

#include <volition/common.h>
#include <volition/Accessors.h>
#include <volition/Block.h>

namespace Volition {

class BlockTree;
class BlockTreeNode;

//================================================================//
// BlockTreeSegment
//================================================================//
class BlockTreeSegment {
public:

    shared_ptr < const BlockTreeNode >  mHead;
    shared_ptr < const BlockTreeNode >  mTail;
    shared_ptr < const BlockTreeNode >  mTop;
    
    //----------------------------------------------------------------//
    size_t          getDefeatCount          ( time_t window ) const;
    size_t          getFullLength           () const;
    size_t          getSegLength            () const;
};

//================================================================//
// BlockTreeRoot
//================================================================//
class BlockTreeRoot {
public:

    shared_ptr < const BlockTreeNode >  mRoot;
    BlockTreeSegment                    mSeg0;
    BlockTreeSegment                    mSeg1;
    
    //----------------------------------------------------------------//
    size_t          getSegLength            () const;
};

//================================================================//
// BlockTreeNode
//================================================================//
class BlockTreeNode :
    public enable_shared_from_this < BlockTreeNode > {
public:

    typedef shared_ptr < BlockTreeNode >            Ptr;
    typedef shared_ptr < const BlockTreeNode >      ConstPtr;

    enum Status {
        STATUS_NEW          = 0x01,
        STATUS_COMPLETE     = 0x02,
        STATUS_MISSING      = 0x04,
        STATUS_INVALID      = 0x08,
    };
    
    enum RewriteMode {
        REWRITE_NONE,
        REWRITE_WINDOW,
        REWRITE_ANY,
    };

private:

    friend class BlockTree;
    friend class BlockTreeTag;

    BlockTree*                              mTree;

    shared_ptr < const BlockHeader >        mHeader;
    shared_ptr < const Block >              mBlock;
    shared_ptr < BlockTreeNode >            mParent;
    set < BlockTreeNode* >                  mChildren;

    Status                                  mStatus;

    //----------------------------------------------------------------//
    shared_ptr < const BlockTreeNode >      findInsertionRecurse    ( shared_ptr < const BlockTreeNode > tail, string minerID, const Digest& visage ) const;
    void                                    logBranchRecurse        ( string& str ) const;
    void                                    mark                    ( BlockTreeNode::Status status );
    void                                    markComplete            ();

public:

    //----------------------------------------------------------------//
    const BlockHeader& operator * () const {
        assert ( this->mHeader );
        return *this->mHeader;
    }

    //----------------------------------------------------------------//
                                            BlockTreeNode           ();
                                            ~BlockTreeNode          ();
    bool                                    checkStatus             ( Status status ) const;
    static int                              compare                 ( shared_ptr < const BlockTreeNode > node0, shared_ptr < const BlockTreeNode > node1, RewriteMode rewriteMode, time_t window );
    shared_ptr < const BlockTreeNode >      findFirstIncomplete     () const;
    shared_ptr < const BlockTreeNode >      findInsertion           ( string minerID, const Digest& visage ) const;
    static BlockTreeRoot                    findRoot                ( shared_ptr < const BlockTreeNode > node0, shared_ptr < const BlockTreeNode > node1 );
    shared_ptr < const Block >              getBlock                () const;
    shared_ptr < const BlockHeader >        getBlockHeader          () const;
    shared_ptr < const BlockTreeNode >      getParent               () const;
    Status                                  getStatus               () const;
    bool                                    isAncestorOf            ( ConstPtr tail ) const;
    BlockTreeNode::ConstPtr                 trim                    ( Status status ) const;
    string                                  writeBranch             () const;
};

//================================================================//
// BlockTree
//================================================================//
class BlockTree :
    public enable_shared_from_this < BlockTree > {
private:

    friend class BlockTreeNode;

    shared_ptr < BlockTreeNode >        mRoot;
    map < string, BlockTreeNode* >      mNodes;

    //----------------------------------------------------------------//
    BlockTreeNode::Ptr                  findNodeForHash         ( string hash );
    void                                logTreeRecurse          ( string prefix, size_t maxDepth, const BlockTreeNode* node, size_t depth ) const;

public:

    GET ( BlockTreeNode::ConstPtr, Root, mRoot )

    //----------------------------------------------------------------//
    BlockTreeNode::ConstPtr             affirmBlock             ( shared_ptr < const Block > block );
    BlockTreeNode::ConstPtr             affirmBlock             ( shared_ptr < const BlockHeader > header, shared_ptr < const Block > block );
                                        BlockTree               ();
                                        ~BlockTree              ();
    BlockTreeNode::ConstPtr             findNodeForHash         ( string hash ) const;
    void                                logTree                 ( string prefix = "", size_t maxDepth = 0 ) const;
    void                                mark                    ( BlockTreeNode::ConstPtr node, BlockTreeNode::Status status );
    BlockTreeNode::ConstPtr             update                  ( shared_ptr < const Block > block );
};

} // namespace Volition
#endif
