// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_BLOCKTREECURSOR_H
#define VOLITION_BLOCKTREECURSOR_H

#include <volition/common.h>
#include <volition/Accessors.h>
#include <volition/Block.h>

namespace Volition {

class AbstractBlockTree;

//================================================================//
// BlockTreeCursor
//================================================================//
class BlockTreeCursor :
    public HasBlockHeaderFields {
public:

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

protected:

    friend class BlockTreeTag;

    AbstractBlockTree*                  mTree;

    shared_ptr < const BlockHeader >    mHeader;
    shared_ptr < const Block >          mBlock;
    Status                              mStatus;
    Meta                                mMeta;

    //----------------------------------------------------------------//
    void                                logBranchRecurse            ( string& str ) const;

    //----------------------------------------------------------------//
    const BlockHeaderFields&            HasBlockHeader_getFields    () const override;

public:

    GET_SET ( AbstractBlockTree*,   Tree,       mTree )

    //----------------------------------------------------------------//
                                        BlockTreeCursor             ();
                                        ~BlockTreeCursor            ();
    bool                                checkStatus                 ( Status status ) const;
    bool                                checkTree                   ( const AbstractBlockTree* tree ) const;
    static int                          compare                     ( const BlockTreeCursor& node0, const BlockTreeCursor& node1, RewriteMode rewriteMode );
    bool                                equals                      ( const BlockTreeCursor& rhs ) const;
    static BlockTreeCursor              findRoot                    ( const BlockTreeCursor& node0, const BlockTreeCursor& node1 );
    shared_ptr < const Block >          getBlock                    () const;
    string                              getHash                     () const;
    const BlockHeader&                  getHeader                   () const;
    BlockTreeCursor                     getParent                   () const;
    Status                              getStatus                   () const;
    bool                                hasHeader                   () const;
    bool                                hasParent                   () const;
    bool                                isAncestorOf                ( BlockTreeCursor tail ) const;
    bool                                isComplete                  () const;
    bool                                isInvalid                   () const;
    bool                                isMissing                   () const;
    bool                                isMissingOrInvalid          () const;
    bool                                isNew                       () const;
    bool                                isRefused                   () const;
    BlockTreeCursor                     trim                        ( Status status ) const;
    BlockTreeCursor                     trimInvalid                 () const;
    BlockTreeCursor                     trimMissing                 () const;
    BlockTreeCursor                     trimMissingOrInvalid        () const;
    string                              writeBranch                 () const;
};

} // namespace Volition
#endif
