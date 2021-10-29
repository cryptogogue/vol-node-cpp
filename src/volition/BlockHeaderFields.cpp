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
    mVersion ( 0 ),
    mTime ( 0 ),
    mBlockDelay ( 0 ),
    mRewriteWindow ( 0 ) {
}

//----------------------------------------------------------------//
BlockHeaderFields::~BlockHeaderFields () {
}

} // namespace Volition
