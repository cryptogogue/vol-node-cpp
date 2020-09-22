// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Block.h>
#include <volition/TheContext.h>

namespace Volition {

//================================================================//
// TheContext
//================================================================//

//----------------------------------------------------------------//
TheContext::RewriteMode TheContext::getRewriteMode () const {

    return this->mRewriteMode;
}

//----------------------------------------------------------------//
double TheContext::getWindow () const {

    return this->mRewriteWindowInSeconds;
}

//----------------------------------------------------------------//
TheContext::TheContext () :
    mRewriteMode ( REWRITE_NONE ),
    mRewriteWindowInSeconds ( 0 ) {
}

//----------------------------------------------------------------//
void TheContext::setRewriteMode ( RewriteMode mode ) {

    this->mRewriteMode = mode;
}

//----------------------------------------------------------------//
void TheContext::setRewriteWindow ( double window ) {

    this->mRewriteWindowInSeconds = window;
    this->setRewriteMode ( REWRITE_WINDOW );
}

} // namespace Volition
