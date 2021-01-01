// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_BLOCKTREECURSOR_H
#define VOLITION_BLOCKTREECURSOR_H

#include <volition/common.h>
#include <volition/Accessors.h>
#include <volition/Block.h>

namespace Volition {

class BlockTree;

//================================================================//
// BlockTreeCursor
//================================================================//
class BlockTreeCursor {
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

    friend class BlockTree;
    friend class BlockTreeNodeTag;

    BlockTree*                          mTree;

    shared_ptr < const BlockHeader >    mHeader;
    shared_ptr < const Block >          mBlock;
    Status                              mStatus;
    Meta                                mMeta;

    //----------------------------------------------------------------//
    void                                logBranchRecurse            ( string& str ) const;

public:

    //----------------------------------------------------------------//
    const BlockHeader& operator * () const {
        assert ( this->mHeader );
        return *this->mHeader;
    }

    //----------------------------------------------------------------//
                                        BlockTreeCursor             ();
                                        ~BlockTreeCursor            ();
    bool                                checkStatus                 ( Status status ) const;
    static int                          compare                     ( const BlockTreeCursor& node0, const BlockTreeCursor& node1, RewriteMode rewriteMode );
    bool                                equals                      ( const BlockTreeCursor& rhs ) const;
    bool                                exists                      () const;
    static BlockTreeCursor              findRoot                    ( const BlockTreeCursor& node0, const BlockTreeCursor& node1 );
    shared_ptr < const Block >          getBlock                    () const;
    shared_ptr < const BlockHeader >    getBlockHeader              () const;
    string                              getHash                     () const;
    BlockTreeCursor                     getParent                   () const;
    Status                              getStatus                   () const;
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
