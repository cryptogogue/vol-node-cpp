// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_CHAINPLACEMENT_H
#define VOLITION_CHAINPLACEMENT_H

#include <common.h>

namespace Volition {

class Cycle;

//================================================================//
// ChainPlacement
//================================================================//
class ChainPlacement {
private:

    friend class Chain;
    friend class Cycle;

    bool                    mCanPush;
    const Cycle*            mCycle;

    //----------------------------------------------------------------//
                            ChainPlacement      ();
                            ChainPlacement      ( const Cycle* cycle );

public:

    //----------------------------------------------------------------//
    bool                    canPush             () const;
                            ~ChainPlacement     ();
};

} // namespace Volition
#endif
