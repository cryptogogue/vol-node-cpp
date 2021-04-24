// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_BLOCKTREESAMPLER_H
#define VOLITION_BLOCKTREESAMPLER_H

#include <volition/common.h>
#include <volition/AbstractBlockTree.h>
#include <volition/Accessors.h>
#include <volition/Block.h>
#include <volition/BlockTreeCursor.h>
#include <volition/BlockTreeEnums.h>
#include <volition/BlockTreeTag.h>

namespace Volition {

//================================================================//
// BlockTreeSamplerNode
//================================================================//
class BlockTreeSamplerNode {
public:

    list < BlockTreeCursor >        mLeaves;
    list < BlockTreeCursor >        mChildren;
    BlockTreeCursor                 mRoot;

    //----------------------------------------------------------------//
                                BlockTreeSamplerNode        ();
                                BlockTreeSamplerNode        ( const BlockTreeCursor& cursor );
};

//================================================================//
// BlockTreeSampler
//================================================================//
class BlockTreeSampler {
protected:

    //----------------------------------------------------------------//
    static u64                  findHeight                  ( const list < BlockTreeSamplerNode >& nodes );

public:

    list < BlockTreeCursor >    mLeaves;

    //----------------------------------------------------------------//
    void                        addLeaf                     ( const BlockTreeCursor& leaf );
                                BlockTreeSampler            ();
                                ~BlockTreeSampler           ();
    BlockTreeSamplerNode        sample                      ( double threshold = 0.5 ) const;
};

} // namespace Volition
#endif
