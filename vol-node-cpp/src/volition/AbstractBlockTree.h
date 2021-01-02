// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_ABSTRACTBLOCKTREE_H
#define VOLITION_ABSTRACTBLOCKTREE_H

#include <volition/common.h>
#include <volition/AbstractBlockTree.h>
#include <volition/Accessors.h>
#include <volition/Block.h>
#include <volition/BlockTreeCursor.h>
#include <volition/BlockTreeTag.h>

namespace Volition {

//================================================================//
// AbstractBlockTree
//================================================================//
class AbstractBlockTree {
public:

    enum CanAppend {
        APPEND_OK,
        ALREADY_EXISTS,
        MISSING_PARENT,
        REFUSED,
        TOO_SOON,
    };

protected:

    friend class BlockTreeCursor;
    friend class BlockTreeTag;

    //----------------------------------------------------------------//
    BlockTreeCursor             affirm                  ( BlockTreeTag& tag, shared_ptr < const BlockHeader > header, shared_ptr < const Block > block, bool isProvisional = false );
    void                        setTagTree              ( BlockTreeTag& tag );

    //----------------------------------------------------------------//
    virtual BlockTreeCursor         AbstractBlockTree_affirm                    ( BlockTreeTag& tag, shared_ptr < const BlockHeader > header, shared_ptr < const Block > block, bool isProvisional = false ) = 0;
    virtual CanAppend               AbstractBlockTree_checkAppend               ( const BlockHeader& header ) const = 0;
    virtual int                     AbstractBlockTree_compare                   ( const BlockTreeCursor& cursor0, const BlockTreeCursor& cursor1, BlockTreeCursor::RewriteMode rewriteMode ) const = 0;
    virtual BlockTreeCursor         AbstractBlockTree_findCursorForHash         ( string hash ) const = 0;
    virtual BlockTreeCursor         AbstractBlockTree_findCursorForTag          ( const BlockTreeTag& tag ) const = 0;
    virtual BlockTreeCursor         AbstractBlockTree_findRoot                  ( const BlockTreeCursor& cursor0, const BlockTreeCursor& cursor1 ) const = 0;
    virtual BlockTreeCursor         AbstractBlockTree_getParent                 ( const BlockTreeCursor& cursor ) const = 0;
    virtual void                    AbstractBlockTree_mark                      ( const BlockTreeCursor& cursor, BlockTreeCursor::Status status ) = 0;
    virtual BlockTreeCursor         AbstractBlockTree_tag                       ( BlockTreeTag& tag, const BlockTreeCursor& cursor ) = 0;
    virtual BlockTreeCursor         AbstractBlockTree_tag                       ( BlockTreeTag& tag, const BlockTreeTag& otherTag ) = 0;
    virtual BlockTreeCursor         AbstractBlockTree_update                    ( shared_ptr < const Block > block ) = 0;

public:

    //----------------------------------------------------------------//
    BlockTreeCursor             affirmBlock             ( BlockTreeTag& tag, shared_ptr < const Block > block );
    BlockTreeCursor             affirmHeader            ( BlockTreeTag& tag, shared_ptr < const BlockHeader > header );
    BlockTreeCursor             affirmProvisional       ( BlockTreeTag& tag, shared_ptr < const BlockHeader > header );
                                AbstractBlockTree       ();
    virtual                     ~AbstractBlockTree      ();
    CanAppend                   checkAppend             ( const BlockHeader& header ) const;
    int                         compare                 ( const BlockTreeCursor& cursor0, const BlockTreeCursor& cursor1, BlockTreeCursor::RewriteMode rewriteMode ) const;
    BlockTreeCursor             findCursorForHash       ( string hash ) const;
    BlockTreeCursor             findCursorForTag        ( const BlockTreeTag& tag ) const;
    BlockTreeCursor             findRoot                ( const BlockTreeCursor& cursor0, const BlockTreeCursor& cursor1 ) const;
    BlockTreeCursor             getParent               ( const BlockTreeCursor& cursor ) const;
    void                        mark                    ( const BlockTreeCursor& cursor, BlockTreeCursor::Status status );
    BlockTreeCursor             tag                     ( BlockTreeTag& tag, const BlockTreeCursor& cursor );
    BlockTreeCursor             tag                     ( BlockTreeTag& tag, const BlockTreeTag& otherTag );
    BlockTreeCursor             update                  ( shared_ptr < const Block > block );
};

} // namespace Volition
#endif
