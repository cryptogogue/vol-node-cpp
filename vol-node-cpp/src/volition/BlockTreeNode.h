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
// BlockTreeNode
//================================================================//
class BlockTreeNode :
    public enable_shared_from_this < BlockTreeNode > {
public:

    typedef BlockTreeNode*              Ptr;
    typedef const BlockTreeNode*        ConstPtr;

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
    friend class BlockTreeNode;
    friend class BlockTreeNodeTag;

    BlockTree*                          mTree;

    shared_ptr < const BlockHeader >    mHeader;
    shared_ptr < const Block >          mBlock;
    shared_ptr < BlockTreeNode >        mParent;
    set < BlockTreeNode* >              mChildren;

    Status                              mStatus;
    Meta                                mMeta;

    //----------------------------------------------------------------//
    void                                clearParent             ();
    static BlockTreeFork                findFork                ( const BlockTreeNode* node0, const BlockTreeNode* node1 );
    void                                logBranchRecurse        ( string& str ) const;
    void                                mark                    ( BlockTreeNode::Status status );
    void                                markComplete            ();
    void                                markRefused             ();

public:

    //----------------------------------------------------------------//
    const BlockHeader& operator * () const {
        assert ( this->mHeader );
        return *this->mHeader;
    }

    //----------------------------------------------------------------//
                                        BlockTreeNode               ();
                                        ~BlockTreeNode              ();
    bool                                checkStatus                 ( Status status ) const;
    static int                          compare                     ( const BlockTreeNode* node0, const BlockTreeNode* node1, RewriteMode rewriteMode );
    static const BlockTreeNode*         findRoot                    ( const BlockTreeNode* node0, const BlockTreeNode* node1 );
    shared_ptr < const Block >          getBlock                    () const;
    shared_ptr < const BlockHeader >    getBlockHeader              () const;
    const BlockTreeNode*                getParent                   () const;
    BlockTreeNode::Status               getStatus                   () const;
    bool                                isAncestorOf                ( const BlockTreeNode* tail ) const;
    bool                                isComplete                  () const;
    bool                                isInvalid                   () const;
    bool                                isMissing                   () const;
    bool                                isMissingOrInvalid          () const;
    bool                                isNew                       () const;
    bool                                isRefused                   () const;
    const BlockTreeNode*                trim                        ( Status status ) const;
    const BlockTreeNode*                trimInvalid                 () const;
    const BlockTreeNode*                trimMissing                 () const;
    const BlockTreeNode*                trimMissingOrInvalid        () const;
    string                              writeBranch                 () const;
};

} // namespace Volition
#endif
