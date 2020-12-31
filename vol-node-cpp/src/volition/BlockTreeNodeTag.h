// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_BLOCKTREENODETAG_H
#define VOLITION_BLOCKTREENODETAG_H

#include <volition/common.h>
#include <volition/BlockTreeCursor.h>

namespace Volition {

class BlockTree;
class BlockTreeNode;

//================================================================//
// BlockTreeNodeTag
//================================================================//
class BlockTreeNodeTag {
private:

    friend class BlockTree;

    string                      mTagName;
    BlockTree*                  mBlockTree;

public:

    //----------------------------------------------------------------//
    BlockTreeNodeTag&           operator =                          ( BlockTreeCursor other );
    BlockTreeNodeTag&           operator =                          ( const BlockTreeNodeTag& other );
    bool                        operator ==                         ( const BlockTreeNodeTag& other ) const;
    BlockTreeCursor             operator *                          () const;
                                operator bool                       () const;
                                BlockTreeNodeTag                    ();
                                BlockTreeNodeTag                    ( BlockTreeNodeTag& other );
                                ~BlockTreeNodeTag                   ();
    BlockTreeCursor             get                                 () const;
    void                        setTagName                          ( string tagName );
};

} // namespace Volition
#endif
