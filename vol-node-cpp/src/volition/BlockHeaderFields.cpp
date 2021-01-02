// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/BlockHeaderFields.h>
#include <volition/Ledger.h>

namespace Volition {

//================================================================//
// BlockHeaderFields
//================================================================//

//----------------------------------------------------------------//
BlockHeaderFields::BlockHeaderFields () :
    mHeight ( 0 ),
    mTime ( 0 ),
    mBlockDelay ( Ledger::DEFAULT_BLOCK_DELAY_IN_SECONDS ),
    mRewriteWindow ( Ledger::DEFAULT_REWRITE_WINDOW_IN_SECONDS ) {
}

//----------------------------------------------------------------//
BlockHeaderFields::~BlockHeaderFields () {
}

} // namespace Volition
