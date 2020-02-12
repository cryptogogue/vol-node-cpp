// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/TheWebMiner.h>

namespace Volition {

//================================================================//
// TheWebMiner
//================================================================//

//----------------------------------------------------------------//
void TheWebMiner::shutdown () {

    this->mWebMiner.shutdown ();
}

//----------------------------------------------------------------//
TheWebMiner::TheWebMiner () {
}

//----------------------------------------------------------------//
TheWebMiner::~TheWebMiner () {
}

//================================================================//
// ScopedWebMinerLock
//================================================================//

//----------------------------------------------------------------//
WebMiner& ScopedWebMinerLock::getWebMiner () {
    return this->mWebMiner;
}

//----------------------------------------------------------------//
ScopedWebMinerLock::ScopedWebMinerLock ( TheWebMiner& theWebMiner ) :
    mScopedLock ( theWebMiner.mWebMiner.getMutex ()),
    mWebMiner ( theWebMiner.mWebMiner ) {
}

} // namespace Volition
