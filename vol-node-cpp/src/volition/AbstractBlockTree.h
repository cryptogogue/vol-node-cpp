// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_ABSTRACTBLOCKTREE_H
#define VOLITION_ABSTRACTBLOCKTREE_H

#include <volition/common.h>
#include <volition/AbstractBlockTree.h>
#include <volition/Accessors.h>
#include <volition/Block.h>
#include <volition/BlockTreeCursor.h>
#include <volition/BlockTreeEnums.h>
#include <volition/BlockTreeTag.h>

namespace Volition {

//================================================================//
// BlockTreeSegment
//================================================================//
class BlockTreeSegment :
    list < BlockTreeCursor > {
public:

    friend class AbstractBlockTree;

    typedef list < BlockTreeCursor >::const_iterator Iterator;

    Iterator        mHead;
    Iterator        mTail;
    Iterator        mTop;
    
    //----------------------------------------------------------------//
    size_t          getFullLength           () const;
    size_t          getRewriteDefeatCount   () const;
    Iterator        pushFront               ( const BlockTreeCursor& cursor );
};

//================================================================//
// BlockTreeFork
//================================================================//
class BlockTreeFork {
public:

    BlockTreeSegment::Iterator      mRoot;
    BlockTreeSegment                mSeg0;
    BlockTreeSegment                mSeg1;
    
    //----------------------------------------------------------------//
    size_t          getSegLength            () const;
};

//================================================================//
// AbstractBlockTree
//================================================================//
class AbstractBlockTree {
protected:

    friend class BlockTreeCursor;
    friend class BlockTreeTag;
    friend class DebugBlockTree;

    //----------------------------------------------------------------//
    BlockTreeCursor             affirm                  ( BlockTreeTag& tag, shared_ptr < const BlockHeader > header, shared_ptr < const Block > block, bool isProvisional = false );
    void                        findFork                ( BlockTreeFork& fork, BlockTreeCursor cursor0, BlockTreeCursor cursor1 ) const;
    BlockTreeCursor             makeCursor              ( shared_ptr < const BlockHeader > header, kBlockTreeEntryStatus status, kBlockTreeEntryMeta meta, bool hasBlock ) const;

    //----------------------------------------------------------------//
    virtual BlockTreeCursor             AbstractBlockTree_affirm                    ( BlockTreeTag& tag, shared_ptr < const BlockHeader > header, shared_ptr < const Block > block, bool isProvisional ) = 0;
    virtual BlockTreeCursor             AbstractBlockTree_findCursorForHash         ( string hash ) const = 0;
    virtual BlockTreeCursor             AbstractBlockTree_findCursorForTag          ( const BlockTreeTag& tag ) const = 0;
    virtual shared_ptr < const Block >  AbstractBlockTree_getBlock                  ( const BlockTreeCursor& cursor ) const = 0;
    virtual void                        AbstractBlockTree_mark                      ( const BlockTreeCursor& cursor, kBlockTreeEntryStatus status ) = 0;
    virtual BlockTreeCursor             AbstractBlockTree_tag                       ( BlockTreeTag& tag, const BlockTreeCursor& cursor ) = 0;
    virtual BlockTreeCursor             AbstractBlockTree_tag                       ( BlockTreeTag& tag, const BlockTreeTag& otherTag ) = 0;
    virtual void                        AbstractBlockTree_update                    ( shared_ptr < const Block > block ) = 0;

public:

    //----------------------------------------------------------------//
    BlockTreeCursor             affirmBlock             ( BlockTreeTag& tag, shared_ptr < const Block > block );
    BlockTreeCursor             affirmHeader            ( BlockTreeTag& tag, shared_ptr < const BlockHeader > header );
    BlockTreeCursor             affirmProvisional       ( BlockTreeTag& tag, shared_ptr < const BlockHeader > header );
                                AbstractBlockTree       ();
    virtual                     ~AbstractBlockTree      ();
    kBlockTreeAppendResult      checkAppend             ( const BlockHeader& header ) const;
    static bool                 checkStatusTransition   ( kBlockTreeEntryStatus from, kBlockTreeEntryStatus to );
    int                         compare                 ( const BlockTreeCursor& cursor0, const BlockTreeCursor& cursor1 ) const;
    BlockTreeCursor             findCursorForHash       ( string hash ) const;
    BlockTreeCursor             findCursorForTag        ( const BlockTreeTag& tag ) const;
    BlockTreeCursor             findRoot                ( const BlockTreeCursor& cursor0, const BlockTreeCursor& cursor1 ) const;
    shared_ptr < const Block >  getBlock                ( const BlockTreeCursor& cursor ) const;
    BlockTreeCursor             getParent               ( const BlockTreeCursor& cursor ) const;
    void                        mark                    ( const BlockTreeCursor& cursor, kBlockTreeEntryStatus status );
    BlockTreeCursor             tag                     ( BlockTreeTag& tag, const BlockTreeCursor& cursor );
    BlockTreeCursor             tag                     ( BlockTreeTag& tag, const BlockTreeTag& otherTag );
    void                        update                  ( shared_ptr < const Block > block );
};

} // namespace Volition
#endif
