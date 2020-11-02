// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Block.h>
#include <volition/Schema.h>
#include <volition/TheContext.h>

namespace Volition {

//================================================================//
// TheContext
//================================================================//

//----------------------------------------------------------------//
TheContext::ScoringMode TheContext::getScoringMode () const {

    return this->mScoringMode;
}

//----------------------------------------------------------------//
size_t TheContext::getScoringModulo () const {

    return this->mScoringModulo;
}

//----------------------------------------------------------------//
double TheContext::getWindow () const {

    return this->mRewriteWindowInSeconds;
}

//----------------------------------------------------------------//
TheContext::TheContext () :
    mScoringMode ( ScoringMode::ALLURE ),
    mScoringModulo ( 0 ),
    mRewriteWindowInSeconds ( 0 ) {
}

//----------------------------------------------------------------//
void TheContext::setScoringMode ( ScoringMode scoringMode, size_t modulo ) {

    this->mScoringMode = scoringMode;
    this->mScoringModulo = modulo;
}

//----------------------------------------------------------------//
void TheContext::setWindow ( double window ) {

    this->mRewriteWindowInSeconds = window;
}

} // namespace Volition
