// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_BLOCKTREENODE_H
#define VOLITION_BLOCKTREENODE_H

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
    size_t          getFullLength           () const;
    size_t          getRewriteDefeatCount   () const;
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

    enum Meta {
        META_NONE,
        META_PROVISIONAL,
        META_REFUSED,
    };

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
    Meta                                    mMeta;

    //----------------------------------------------------------------//
    void                                    clearParent             ();
    shared_ptr < const BlockTreeNode >      findInsertionRecurse    ( shared_ptr < const BlockTreeNode > tail, string minerID, const Digest& visage ) const;
    void                                    logBranchRecurse        ( string& str ) const;
    void                                    mark                    ( BlockTreeNode::Status status );
    void                                    markComplete            ();
    void                                    markRefused             ();

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
    static int                              compare                 ( shared_ptr < const BlockTreeNode > node0, shared_ptr < const BlockTreeNode > node1, RewriteMode rewriteMode );
    shared_ptr < const BlockTreeNode >      findFirstIncomplete     () const;
    shared_ptr < const BlockTreeNode >      findInsertion           ( string minerID, const Digest& visage ) const;
    static BlockTreeRoot                    findRoot                ( shared_ptr < const BlockTreeNode > node0, shared_ptr < const BlockTreeNode > node1 );
    shared_ptr < const Block >              getBlock                () const;
    shared_ptr < const BlockHeader >        getBlockHeader          () const;
    shared_ptr < const BlockTreeNode >      getParent               () const;
    Status                                  getStatus               () const;
    bool                                    isAncestorOf            ( ConstPtr tail ) const;
    bool                                    isComplete              () const;
    bool                                    isInvalid               () const;
    bool                                    isMissing               () const;
    bool                                    isMissingOrInvalid      () const;
    bool                                    isNew                   () const;
    bool                                    isRefused               () const;
    BlockTreeNode::ConstPtr                 trim                    ( Status status ) const;
    BlockTreeNode::ConstPtr                 trimInvalid             () const;
    BlockTreeNode::ConstPtr                 trimMissing             () const;
    BlockTreeNode::ConstPtr                 trimMissingOrInvalid    () const;
    string                                  writeBranch             () const;
    string                                  writeCharmTag           () const;
};

} // namespace Volition
#endif
