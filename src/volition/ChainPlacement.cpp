// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/ChainPlacement.h>
#include <volition/Cycle.h>

namespace Volition {

//================================================================//
// ChainPlacement
//================================================================//

//----------------------------------------------------------------//
ChainPlacement::ChainPlacement ( const Cycle& cycle, bool newCycle ) :
    mNewCycle ( newCycle ),
    mCycle ( &cycle ) {
}

//----------------------------------------------------------------//
ChainPlacement::~ChainPlacement () {
}

//----------------------------------------------------------------//
size_t ChainPlacement::getCycleID () const {

    assert ( this->mCycle );
    return this->mCycle->mCycleID + ( this->mNewCycle ? 1 : 0 );
}

} // namespace Volition
