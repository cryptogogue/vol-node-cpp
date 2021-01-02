// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_INMEMORYBLOCKTREENODE_H
#define VOLITION_INMEMORYBLOCKTREENODE_H

#include <volition/common.h>
#include <volition/Accessors.h>
#include <volition/Block.h>

namespace Volition {

//================================================================//
// InMemoryBlockTreeNode
//================================================================//
class InMemoryBlockTreeNode :
    public BlockTreeCursor,
    public enable_shared_from_this < InMemoryBlockTreeNode > {
public:

    typedef InMemoryBlockTreeNode*              Ptr;
    typedef const InMemoryBlockTreeNode*        ConstPtr;

private:

    friend class InMemoryBlockTree;

    shared_ptr < InMemoryBlockTreeNode >        mParent;
    set < InMemoryBlockTreeNode* >              mChildren;

    //----------------------------------------------------------------//
    void            clearParent                 ();
    void            mark                        ( kBlockTreeEntryStatus status );
    void            markComplete                ();
    void            markRefused                 ();

public:

    //----------------------------------------------------------------//
                    InMemoryBlockTreeNode       ();
                    ~InMemoryBlockTreeNode      ();
};

} // namespace Volition
#endif
