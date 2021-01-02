// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_BLOCKTREETAG_H
#define VOLITION_BLOCKTREETAG_H

#include <volition/common.h>
#include <volition/Accessors.h>
#include <volition/BlockTreeCursor.h>

namespace Volition {

class AbstractBlockTree;

//================================================================//
// BlockTreeTag
//================================================================//
class BlockTreeTag :
    public HasBlockHeaderFields {
private:

    friend class AbstractBlockTree;

    AbstractBlockTree*          mTree;
    string                      mName;
    
    //----------------------------------------------------------------//
    const BlockHeaderFields&    HasBlockHeader_getFields        () const override;

public:

    GET_SET ( string,               Name,       mName )
    GET_SET ( AbstractBlockTree*,   Tree,       mTree )

    //----------------------------------------------------------------//
    BlockTreeTag&               operator =                      ( BlockTreeCursor other );
    BlockTreeTag&               operator =                      ( const BlockTreeTag& other );
    BlockTreeCursor             operator *                      () const;
                                BlockTreeTag                    ();
                                BlockTreeTag                    ( BlockTreeTag& other );
                                ~BlockTreeTag                   ();
    bool                        checkTree                       ( const AbstractBlockTree* tree ) const;
    bool                        equals                          ( const BlockTreeTag& rhs ) const;
    BlockTreeCursor             getCursor                       () const;
    bool                        hasCursor                       () const;
    bool                        hasName                         () const;
};

} // namespace Volition
#endif
