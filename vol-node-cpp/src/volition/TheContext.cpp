// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Block.h>
#include <volition/TheContext.h>

namespace Volition {

//================================================================//
// TheContext
//================================================================//

//----------------------------------------------------------------//
double TheContext::getWindow () const {

    return this->mRewriteWindowInSeconds;
}

//----------------------------------------------------------------//
TheContext::TheContext () :
    mRewriteWindowInSeconds ( 0 ) {
}

//----------------------------------------------------------------//
void TheContext::setWindow ( double window ) {

    this->mRewriteWindowInSeconds = window;
}

} // namespace Volition
