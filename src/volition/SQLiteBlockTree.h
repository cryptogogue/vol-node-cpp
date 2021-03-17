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

    mutable SQLite                      mDB;

    //----------------------------------------------------------------//
    int                                 getNodeIDFromHash               ( string hash ) const;
    int                                 getNodeIDFromTagName            ( string tagName ) const;
    kBlockTreeBranchStatus              getNodeBranchStatus             ( int nodeID, kBlockTreeBranchStatus status = kBlockTreeBranchStatus::BRANCH_STATUS_INVALID ) const;
    void                                pruneUnreferencedNodes          ();
    BlockTreeCursor                     readCursor                      ( const SQLiteStatement& stmt ) const;
    void                                setBranchStatus                 ( int nodeID, const Digest& parentDigest, kBlockTreeBranchStatus status );
    void                                setBranchStatusInner            ( int nodeID, kBlockTreeBranchStatus status, set < int >& queue );
    void                                setSearchStatus                 ( int nodeID, kBlockTreeSearchStatus status );
    void                                setTag                          ( string tagName, int nodeID );
    static string                       stringFromBranchStatus          ( kBlockTreeBranchStatus status );
    static string                       stringFromSearchStatus          ( kBlockTreeSearchStatus status );
    static kBlockTreeBranchStatus       stringToBranchStatus            ( string str );
    static kBlockTreeSearchStatus       stringToSearchStatus            ( string str );

    //----------------------------------------------------------------//
    BlockTreeCursor                     AbstractBlockTree_affirm                    ( BlockTreeTag& tag, shared_ptr < const BlockHeader > header, shared_ptr < const Block > block, bool isProvisional = false ) override;
    BlockTreeCursor                     AbstractBlockTree_findCursorForHash         ( string hash ) const override;
    BlockTreeCursor                     AbstractBlockTree_findCursorForTagName      ( string tagName ) const override;
    shared_ptr < const Block >          AbstractBlockTree_getBlock                  ( const BlockTreeCursor& cursor ) const override;
    void                                AbstractBlockTree_setBranchStatus           ( const BlockTreeCursor& cursor, kBlockTreeBranchStatus status ) override;
    void                                AbstractBlockTree_setSearchStatus           ( const BlockTreeCursor& cursor, kBlockTreeSearchStatus status ) override;
    BlockTreeCursor                     AbstractBlockTree_tag                       ( BlockTreeTag& tag, const BlockTreeCursor& cursor ) override;
    BlockTreeCursor                     AbstractBlockTree_tag                       ( BlockTreeTag& tag, const BlockTreeTag& otherTag ) override;
    void                                AbstractBlockTree_update                    ( shared_ptr < const Block > block ) override;

public:

    //----------------------------------------------------------------//
                                        SQLiteBlockTree                 ( string filename );
                                        ~SQLiteBlockTree                ();
};

} // namespace Volition
#endif
