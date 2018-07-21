//
//  main.cpp
// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Block.h>
#include <volition/TheContext.h>

namespace Volition {

static const char* PUBLIC_KEY_STRING    = "-----BEGIN PUBLIC KEY-----\nMFYwEAYHKoZIzj0CAQYFK4EEAAoDQgAEzfUD/qtmfa4H2NJ+vriC8SNm0bUe8dMx\nBlOig2FzzOZE1iVu3u1Tgvvr4MsYmY2KYxOt+zHWcT5DlSKmrmaQqw==\n-----END PUBLIC KEY-----\n";
static const char* DIGEST_STRING        = "9ec61f3245d657536923fc88b526cbda3b280612b7113087b0f18894b69039f4";

//================================================================//
// TheContext
//================================================================//

//----------------------------------------------------------------//
const Poco::DigestEngine::Digest& TheContext::getGenesisBlockDigest () const {

    assert ( this->mDigest );
    return *this->mDigest;
}

//----------------------------------------------------------------//
const Poco::Crypto::ECKey& TheContext::getGenesisBlockKey () const {

    assert ( this->mKey );
    return *this->mKey;
}

//----------------------------------------------------------------//
TheContext::ScoringMode TheContext::getScoringMode () const {

    return this->mScoringMode;
}

//----------------------------------------------------------------//
TheContext::TheContext () :
    mScoringMode ( ScoringMode::ALLURE ) {

    stringstream genesisKeyStream ( PUBLIC_KEY_STRING );
    this->mKey = make_unique < Poco::Crypto::ECKey >( &genesisKeyStream );
    this->mDigest = make_unique < Poco::DigestEngine::Digest >( Poco::DigestEngine::digestFromHex ( DIGEST_STRING ));
}

//----------------------------------------------------------------//
void TheContext::setGenesisBlockDigest ( const Poco::DigestEngine::Digest& digest ) {

    this->mDigest = make_unique < Poco::DigestEngine::Digest >( digest );
}

//----------------------------------------------------------------//
void TheContext::setGenesisBlockKey ( const Poco::Crypto::ECKey& key ) {

    this->mKey = make_unique < Poco::Crypto::ECKey >( key );
}

//----------------------------------------------------------------//
void TheContext::setScoringMode ( ScoringMode scoringMode ) {

    this->mScoringMode = scoringMode;
}

} // namespace Volition
