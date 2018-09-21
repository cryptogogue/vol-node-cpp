// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_CHAINPLACEMENT_H
#define VOLITION_CHAINPLACEMENT_H

#include <volition/common.h>
#include <volition/Cycle.h>

namespace Volition {

//================================================================//
// ChainPlacement
//================================================================//
class ChainPlacement {
private:

    friend class Chain;
    friend class Cycle;

    bool            mNewCycle;
    Cycle           mCycle;

    //----------------------------------------------------------------//
                    ChainPlacement      ( const Cycle& cycle, bool newCycle );

public:

    //----------------------------------------------------------------//
                    ~ChainPlacement     ();
    size_t          getCycleID          () const;
};

} // namespace Volition
#endif
