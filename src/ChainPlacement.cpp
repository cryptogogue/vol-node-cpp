// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <ChainPlacement.h>
#include <Cycle.h>

namespace Volition {

//================================================================//
// ChainPlacement
//================================================================//

//----------------------------------------------------------------//
bool ChainPlacement::canPush () const {

    return this->mCanPush;
}

//----------------------------------------------------------------//
ChainPlacement::ChainPlacement () :
    mCanPush ( false ),
    mCycle ( 0 ) {
}

//----------------------------------------------------------------//
ChainPlacement::ChainPlacement ( const Cycle* cycle ) :
    mCanPush ( true ),
    mCycle ( cycle ) {
}

//----------------------------------------------------------------//
ChainPlacement::~ChainPlacement () {
}

} // namespace Volition
