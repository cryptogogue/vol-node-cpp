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
    BlockTreeCursor             operator *                          () const;
                                BlockTreeNodeTag                    ();
                                BlockTreeNodeTag                    ( BlockTreeNodeTag& other );
                                ~BlockTreeNodeTag                   ();
    bool                        equals                              ( const BlockTreeNodeTag& rhs ) const;
    bool                        exists                              () const;
    BlockTreeCursor             get                                 () const;
    void                        setTagName                          ( string tagName );
};

} // namespace Volition
#endif
