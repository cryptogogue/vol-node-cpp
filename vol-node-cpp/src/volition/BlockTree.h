// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_BLOCKTREE_H
#define VOLITION_BLOCKTREE_H

#include <volition/common.h>
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
    size_t          getDefeatCount          () const;
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

private:

    friend class BlockTree;
    friend class BlockTreeTag;

    BlockTree*                              mTree;

    shared_ptr < const BlockHeader >        mHeader;
    shared_ptr < const Block >              mBlock;
    shared_ptr < BlockTreeNode >            mParent;
    set < BlockTreeNode* >                  mChildren;

    enum Status {
        STATUS_PENDING,
        STATUS_EXPIRED,
        STATUS_COMPLETE,
    };

    Status                                  mStatus;

    //----------------------------------------------------------------//
    shared_ptr < const BlockTreeNode >      findInsertionRecurse    ( shared_ptr < const BlockTreeNode > tail, string minerID, const Digest& visage ) const;
    void                                    logBranchRecurse        ( string& str ) const;
    void                                    markComplete            ();
    void                                    markExpired             ();

public:

    //----------------------------------------------------------------//
    const BlockHeader& operator * () const {
        assert ( this->mHeader );
        return *this->mHeader;
    }

    //----------------------------------------------------------------//
                                            BlockTreeNode           ();
                                            ~BlockTreeNode          ();
    static int                              compare                 ( shared_ptr < const BlockTreeNode > node0, shared_ptr < const BlockTreeNode > node1 );
    shared_ptr < const BlockTreeNode >      findInsertion           ( string minerID, const Digest& visage ) const;
    static BlockTreeRoot                    findRoot                ( shared_ptr < const BlockTreeNode > node0, shared_ptr < const BlockTreeNode > node1 );
    shared_ptr < const Block >              getBlock                () const;
    shared_ptr < const BlockHeader >        getBlockHeader          () const;
    shared_ptr < const BlockTreeNode >      getParent               () const;
    bool                                    isAncestorOf            ( ConstPtr tail ) const;
    bool                                    isComplete              () const;
    bool                                    isExpired               () const;
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

    //----------------------------------------------------------------//
    BlockTreeNode::ConstPtr             affirmBlock             ( shared_ptr < const Block > block );
    BlockTreeNode::ConstPtr             affirmBlock             ( shared_ptr < const BlockHeader > header, shared_ptr < const Block > block );
                                        BlockTree               ();
                                        ~BlockTree              ();
    BlockTreeNode::ConstPtr             findNodeForHash         ( string hash ) const;
    void                                logTree                 ( string prefix = "", size_t maxDepth = 0 ) const;
    void                                markExpired             ( BlockTreeNode::ConstPtr node );
};

} // namespace Volition
#endif
