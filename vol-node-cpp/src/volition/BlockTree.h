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
private:

    friend class BlockTree;
    friend class BlockTreeTag;

    BlockTree*                              mTree;
    size_t                                  mTagCount;

    shared_ptr < const BlockHeader >        mHeader;
    shared_ptr < const Block >              mBlock;
    shared_ptr < BlockTreeNode >            mParent;
    set < BlockTreeNode* >                  mChildren;

    //----------------------------------------------------------------//
    void                                    logBranchRecurse        ( string& str ) const;

public:

    //----------------------------------------------------------------//
                                            BlockTreeNode           ();
                                            ~BlockTreeNode          ();
    static BlockTreeRoot                    findRoot                ( shared_ptr < const BlockTreeNode > node0, shared_ptr < const BlockTreeNode > node1 );
    shared_ptr < const Block >              getBlock                () const;
    size_t                                  getHeight               () const;
    shared_ptr < const BlockTreeNode >      getParent               () const;
    time_t                                  getTime                 () const;
    string                                  writeBranch             () const;
};

//================================================================//
// BlockTreeTag
//================================================================//
class BlockTreeTag {
private:

    shared_ptr < BlockTreeNode >        mNode;

    //----------------------------------------------------------------//

public:

    //----------------------------------------------------------------//
    operator bool () const {
        return ( this->mNode != NULL );
    }
    
    //----------------------------------------------------------------//
    operator shared_ptr < const BlockTreeNode > () const {
        return this->mNode;
    }

    //----------------------------------------------------------------//
    BlockTreeTag& operator = ( shared_ptr < BlockTreeNode > node ) {
        this->mark ( node );
        return *this;
    }
    
    //----------------------------------------------------------------//
    BlockTreeTag& operator = ( const BlockTreeTag& other ) {
        this->mark ( other.mNode );
        return *this;
    }

    //----------------------------------------------------------------//
                                            BlockTreeTag            ();
                                            ~BlockTreeTag           ();
    static int                              compare                 ( const BlockTreeTag& tag0, const BlockTreeTag& tag1 );
    size_t                                  getCount                () const;
    shared_ptr < const BlockTreeNode >      getNode                 () const;
    void                                    mark                    ( shared_ptr < BlockTreeNode > node );
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

    //----------------------------------------------------------------//
    void                                logTreeRecurse          ( string prefix, size_t maxDepth, const BlockTreeNode* node, size_t depth ) const;

public:

    //----------------------------------------------------------------//
    shared_ptr < BlockTreeNode >        affirmBlock             ( shared_ptr < const Block > block );
    shared_ptr < BlockTreeNode >        affirmBlock             ( shared_ptr < const BlockHeader > header, shared_ptr < const Block > block );
                                        BlockTree               ();
                                        ~BlockTree              ();
    shared_ptr < BlockTreeNode >        findNodeForHash         ( string hash );
    void                                logTree                 ( string prefix = "", size_t maxDepth = 0 ) const;
};

} // namespace Volition
#endif
