// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SQLITEBLOCKTREE_H
#define VOLITION_SQLITEBLOCKTREE_H

#include <volition/common.h>
#include <volition/AbstractBlockTree.h>
#include <volition/Accessors.h>
#include <volition/Block.h>
#include <volition/BlockTreeCursor.h>
#include <volition/BlockTreeTag.h>

namespace Volition {

//================================================================//
// SQLiteBlockTree
//================================================================//
class SQLiteBlockTree :
    public AbstractBlockTree {
private:

    mutable SQLite              mDB;

    //----------------------------------------------------------------//
    int                                 getNodeIDFromHash               ( string hash ) const;
    int                                 getNodeIDFromTagName            ( string tagName ) const;
    kBlockTreeEntryStatus               getNodeStatus                   ( int nodeID, kBlockTreeEntryStatus status = kBlockTreeEntryStatus::STATUS_INVALID ) const;
    void                                markRecurse                     ( int nodeID, kBlockTreeEntryStatus status );
    void                                pruneUnreferencedNodes          ();
    BlockTreeCursor                     readCursor                      ( const SQLiteStatement& stmt ) const;
    void                                setTag                          ( string tagName, int nodeID );
    static string                       stringFromMeta                  ( kBlockTreeEntryMeta meta );
    static string                       stringFromStatus                ( kBlockTreeEntryStatus status );
    static kBlockTreeEntryMeta          stringToMeta                    ( string str );
    static kBlockTreeEntryStatus        stringToStatus                  ( string str );

    //----------------------------------------------------------------//
    BlockTreeCursor             AbstractBlockTree_affirm                    ( BlockTreeTag& tag, shared_ptr < const BlockHeader > header, shared_ptr < const Block > block, bool isProvisional = false ) override;
    kBlockTreeAppendResult      AbstractBlockTree_checkAppend               ( const BlockHeader& header ) const override;
    int                         AbstractBlockTree_compare                   ( const BlockTreeCursor& cursor0, const BlockTreeCursor& cursor1, kRewriteMode rewriteMode ) const override;
    BlockTreeCursor             AbstractBlockTree_findCursorForHash         ( string hash ) const override;
    BlockTreeCursor             AbstractBlockTree_findCursorForTag          ( const BlockTreeTag& tag ) const override;
    BlockTreeCursor             AbstractBlockTree_findRoot                  ( const BlockTreeCursor& cursor0, const BlockTreeCursor& cursor1 ) const override;
    void                        AbstractBlockTree_mark                      ( const BlockTreeCursor& cursor, kBlockTreeEntryStatus status ) override;
    BlockTreeCursor             AbstractBlockTree_tag                       ( BlockTreeTag& tag, const BlockTreeCursor& cursor ) override;
    BlockTreeCursor             AbstractBlockTree_tag                       ( BlockTreeTag& tag, const BlockTreeTag& otherTag ) override;
    void                        AbstractBlockTree_update                    ( shared_ptr < const Block > block ) override;

public:

    //----------------------------------------------------------------//
                                SQLiteBlockTree                 ( string filename );
                                ~SQLiteBlockTree                ();
};

} // namespace Volition
#endif
