// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_BLOCKTREETAG_H
#define VOLITION_BLOCKTREETAG_H

#include <volition/common.h>
#include <volition/BlockTreeCursor.h>

namespace Volition {

class BlockTree;
class BlockTreeNode;

//================================================================//
// BlockTreeTag
//================================================================//
class BlockTreeTag :
    public HasBlockHeaderFields {
private:

    friend class BlockTree;

    string                      mTagName;
    BlockTree*                  mBlockTree;

    //----------------------------------------------------------------//
    const BlockHeaderFields&    HasBlockHeader_getFields        () const override;

public:

    //----------------------------------------------------------------//
    BlockTreeTag&               operator =                      ( BlockTreeCursor other );
    BlockTreeTag&               operator =                      ( const BlockTreeTag& other );
    BlockTreeCursor             operator *                      () const;
                                BlockTreeTag                    ();
                                BlockTreeTag                    ( BlockTreeTag& other );
                                ~BlockTreeTag                   ();
    bool                        equals                          ( const BlockTreeTag& rhs ) const;
    BlockTreeCursor             getCursor                       () const;
    bool                        hasCursor                       () const;
    void                        setTagName                      ( string tagName );
};

} // namespace Volition
#endif
